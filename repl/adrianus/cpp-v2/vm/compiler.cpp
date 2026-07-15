#include "compiler.h"
#include "opcode.h"
#include "objects.h"
#include "utils.h"
#include "../builtins_registry.h"
#include "../builtins_registry_names.h"
#include <algorithm>
#include <cstdarg>
#include <iostream>
#include <string>

namespace {

int instruction_width_at(const Instructions& ins, int offset) {
    if (offset < 0 || offset >= ins.size) {
        return 0;
    }
    Definition* def = lookup(ins.bytes[static_cast<size_t>(offset)]);
    if (def == nullptr) {
        return 1;
    }
    int width = 1;
    for (int j = 0; j < def->size; ++j) {
        width += def->operandWidths[j];
    }
    return width;
}

// Jump targets are recorded as absolute offsets in the compiler's single buffer.
// leave_scope() slices [start, end) into a standalone function chunk; operands must
// be rebased by subtracting `extract_start_global` so the VM interprets them 0-based.
void rebase_jump_operands_in_extracted_chunk(Instructions& ins, int extract_start_global) {
    int i = 0;
    while (i < ins.size) {
        unsigned char op = ins.bytes[static_cast<size_t>(i)];
        if (op == static_cast<unsigned char>(OP_JUMP) || op == static_cast<unsigned char>(OP_JUMP_NOT_TRUTHY)) {
            int target = read_uint16(ins, i + 1);
            if (target >= extract_start_global) {
                target -= extract_start_global;
                ins.bytes[static_cast<size_t>(i + 1)] = static_cast<unsigned char>((target >> 8) & 0xFF);
                ins.bytes[static_cast<size_t>(i + 2)] = static_cast<unsigned char>(target & 0xFF);
            }
        }
        int w = instruction_width_at(ins, i);
        if (w <= 0) {
            break;
        }
        i += w;
    }
}

Ad_AST_Def_Statement* program_def_statement(Ad_AST_Node* stmt) {
    if (stmt == nullptr) {
        return nullptr;
    }
    if (stmt->type == ST_DEF_STATEMENT) {
        return static_cast<Ad_AST_Def_Statement*>(stmt);
    }
    if (stmt->type == ST_EXPRESSION_STATEMENT) {
        auto* expr_stmt = static_cast<Ad_AST_ExpressionStatement*>(stmt);
        if (expr_stmt->expression != nullptr && expr_stmt->expression->type == ST_DEF_STATEMENT) {
            return static_cast<Ad_AST_Def_Statement*>(expr_stmt->expression);
        }
    }
    return nullptr;
}

void collect_program_assign_names(Ad_AST_Program* program, std::unordered_set<std::string>& names) {
    if (program == nullptr) {
        return;
    }
    for (Ad_AST_Node* stmt : program->statements) {
        if (stmt->type != ST_EXPRESSION_STATEMENT) {
            continue;
        }
        auto* expr = static_cast<Ad_AST_ExpressionStatement*>(stmt)->expression;
        if (expr == nullptr || expr->type != ST_ASSIGN_STATEMENT) {
            continue;
        }
        auto* assign_stmt = static_cast<Ad_AST_AssignStatement*>(expr);
        if (assign_stmt->name != nullptr && assign_stmt->name->type == ST_IDENTIFIER) {
            names.insert(static_cast<Ad_AST_Identifier*>(assign_stmt->name)->value);
        }
    }
}

void hoist_program_function_names(Ad_AST_Program* program, SymbolTable* symbol_table) {
    if (program == nullptr || symbol_table == nullptr) {
        return;
    }
    for (Ad_AST_Node* stmt : program->statements) {
        Ad_AST_Def_Statement* def_stmt = program_def_statement(stmt);
        if (def_stmt == nullptr || def_stmt->name == nullptr || def_stmt->name->type != ST_IDENTIFIER) {
            continue;
        }
        auto* def_name = static_cast<Ad_AST_Identifier*>(def_stmt->name);
        symbol_table->define(def_name->value);
    }
}

} // namespace

void vm_register_builtin_symbols(SymbolTable* symbol_table) {
    if (symbol_table == nullptr) {
        return;
    }
    for (int i = 0; AD_VM_BUILTIN_NAMES[i] != nullptr; ++i) {
        symbol_table->define_builtin(i, std::string(AD_VM_BUILTIN_NAMES[i]));
    }
}

Compiler::Compiler() {
    constants.clear();
    gc = nullptr;
    CompilationScope main_scope(code.instructions);
    scopes = {main_scope};
    scopeIndex = 0;
    frames.clear();
    frames_index = 0;
    symbol_table = new_symbol_table();
    vm_register_builtin_symbols(symbol_table);
    // bytecode, instructions, and scopes are initialized by their default constructors
}

Compiler::Compiler(GarbageCollector* gc) {
    constants.clear();
    this->gc = gc;
    CompilationScope main_scope(code.instructions);
    scopes = {main_scope};
    scopeIndex = 0;
    frames.clear();
    frames_index = 0;
    symbol_table = new_symbol_table();
    vm_register_builtin_symbols(symbol_table);
    // bytecode, instructions, and scopes are initialized by their default constructors
}

Compiler::~Compiler() {
    while (symbol_table != nullptr) {
        SymbolTable* outer = symbol_table->outer;
        delete symbol_table;
        symbol_table = outer;
    }
}

void Compiler::reset() {
    instructions = Instructions();
    bytecode = Bytecode();
    code.instructions = Instructions();
    constants.clear();
    compiled_classes.clear();
    loop_stack.clear();
    compiling_program_direct_statement = false;
    compiling_function_literal = false;
    bootstrap_global_names.clear();
    program_assign_names.clear();
    scopes = {CompilationScope(code.instructions)};
    scopeIndex = 0;
    while (symbol_table != nullptr) {
        SymbolTable* outer = symbol_table->outer;
        delete symbol_table;
        symbol_table = outer;
    }
    symbol_table = new_symbol_table();
    vm_register_builtin_symbols(symbol_table);
}

void Compiler::emitLoopBreak() {
    if (loop_stack.empty()) {
        std::cerr << "[ Compiler Error ] break outside of loop\n";
        return;
    }
    int pos = emit(opJump, 1, {9999});
    loop_stack.back().pending_break_jumps.push_back(pos);
}

void Compiler::emitLoopContinue() {
    if (loop_stack.empty()) {
        std::cerr << "[ Compiler Error ] continue outside of loop\n";
        return;
    }
    LoopCompilation& cur = loop_stack.back();
    if (cur.is_for) {
        int pos = emit(opJump, 1, {9999});
        cur.pending_continue_jumps.push_back(pos);
    } else {
        emit(opJump, 1, {cur.loop_begin_byte});
    }
}

void Compiler::compile(Ad_AST_Node* node) {
    if (node == nullptr) {
        std::cout << "severe error: node is null" << std::endl;
    } else if (node->type == ST_PROGRAM) {
        Ad_AST_Program* program = (Ad_AST_Program*)node;
        // Match evaluator behavior: top-level defs are all bound before any call runs,
        // so forward references like test1() calling test2() must resolve at compile time.
        hoist_program_function_names(program, symbol_table);
        collect_program_assign_names(program, program_assign_names);
        for (Ad_AST_Node* stmt : program->statements) {
            compiling_program_direct_statement = true;
            compile(stmt);
            compiling_program_direct_statement = false;
        }
    } else if (node->type == ST_EXPRESSION_STATEMENT) {
        Ad_AST_ExpressionStatement* expr_stmt = (Ad_AST_ExpressionStatement*)node;
        if (expr_stmt->expression != nullptr) {
            if (expr_stmt->expression->type == ST_DEF_STATEMENT) {
                // hmmm, dupa multe cautari am gasit fixul asta, ce ciudat mi se pare
                compile(expr_stmt->expression);
            } else if (expr_stmt->expression->type == ST_CLASS_STATEMENT) {
                compile(expr_stmt->expression);
            } else if (expr_stmt->expression->type == ST_ASSIGN_STATEMENT) {
                compile(expr_stmt->expression);
            } else if (expr_stmt->expression->type == ST_WHILE_EXPRESSION ||
                       expr_stmt->expression->type == ST_FOR_EXPRESSION) {
                compile(expr_stmt->expression);
            } else if (expr_stmt->expression->type == ST_PLUS_EQUALS) {
                compile(expr_stmt->expression);
            } else {
                compile(expr_stmt->expression);
                if (compiling_program_direct_statement) {
                    emit(opFileStmtOutput, 0, {});
                } else {
                    emit(opPop, 0, {});
                }
            }
        }
    } else if (node->type == ST_PREFIX_EXPRESSION) {
        Ad_AST_PrefixExpression* prefix_expr = (Ad_AST_PrefixExpression*)node;
        compile(prefix_expr->right);
        if (prefix_expr->_operator == "!") {
            emit(opBang, 0, {});
        } else if (prefix_expr->_operator == "-") {
            emit(opMinus, 0, {});
        } else {
            std::cout << "SEVERE ERROR: prefix expression" << std::endl;
        }
    } else if (node->type == ST_PREFIX_INCREMENT) {
        auto* pre = static_cast<Ad_AST_PrefixIncrement*>(node);
        if (pre->name == nullptr || pre->name->type != ST_IDENTIFIER) {
            std::cerr << "[ Compiler Error ] prefix ++/-- only supported for simple identifiers in the VM\n";
            return;
        }
        auto* ident = static_cast<Ad_AST_Identifier*>(pre->name);
        Symbol* sym = symbol_table->resolve(ident->value);
        if (sym == nullptr) {
            if (symbol_table->outer != nullptr) {
                emit_dynamic_instance_field_lookup(ident->value);
                int one_idx = addConstant(new Ad_Integer_Object(1));
                emit(opConstant, 1, {one_idx});
                if (pre->_operator == "++") {
                    emit(opAdd, 0, {});
                } else if (pre->_operator == "--") {
                    emit(opSub, 0, {});
                } else {
                    std::cerr << "[ Compiler Error ] unsupported prefix increment operator\n";
                    return;
                }
                emit_dynamic_instance_field_patch(ident->value);
                emit_dynamic_instance_field_lookup(ident->value);
                return;
            }
            std::cerr << "[ Compiler Error ] undefined identifier in prefix ++/--: " << ident->value << "\n";
            return;
        }
        if (sym->scope == SymbolScope::FREE) {
            std::cerr << "[ Compiler Error ] prefix ++/-- for captured variables is not supported yet\n";
            return;
        }
        load_symbol(*sym, ident->value);
        int one_idx = addConstant(new Ad_Integer_Object(1));
        emit(opConstant, 1, {one_idx});
        if (pre->_operator == "++") {
            emit(opAdd, 0, {});
        } else if (pre->_operator == "--") {
            emit(opSub, 0, {});
        } else {
            std::cerr << "[ Compiler Error ] unsupported prefix increment operator\n";
            return;
        }
        if (sym->scope == SymbolScope::GLOBAL) {
            emit(opSetGlobal, 1, {sym->index});
            load_symbol(*sym, ident->value);
        } else if (sym->scope == SymbolScope::LOCAL) {
            emit(opSetLocal, 1, {sym->index});
            load_symbol(*sym, ident->value);
        } else if (sym->scope == SymbolScope::CLASS) {
            Ad_String_Object* field_name = new Ad_String_Object(ident->value);
            emit(opConstant, 1, {addConstant(field_name)});
            emit(opPatchPropertySym, 1, {sym->index});
            load_symbol(*sym, ident->value);
        } else {
            std::cerr << "[ Compiler Error ] prefix ++/-- for this symbol scope is not supported in the VM\n";
            return;
        }
    } else if (node->type == ST_POSTFIX_INCREMENT) {
        auto* post = static_cast<Ad_AST_PostfixIncrement*>(node);
        if (post->name == nullptr) {
            std::cerr << "[ Compiler Error ] postfix ++/-- requires a target\n";
            return;
        }
        if (post->name->type == ST_INDEX_EXPRESSION) {
            auto* index_expr = static_cast<Ad_AST_IndexExpression*>(post->name);
            compile(index_expr->left);
            compile(index_expr->index);
            emit(opIndex, 0, {});
            compile(index_expr->left);
            compile(index_expr->index);
            emit(opIndex, 0, {});
            int one_idx = addConstant(new Ad_Integer_Object(1));
            emit(opConstant, 1, {one_idx});
            if (post->_operator == "++") {
                emit(opAdd, 0, {});
            } else if (post->_operator == "--") {
                emit(opSub, 0, {});
            } else {
                std::cerr << "[ Compiler Error ] unsupported postfix increment operator\n";
                return;
            }
            compile(index_expr->left);
            compile(index_expr->index);
            emit(opPostfixIndex, 0, {});
        } else if (post->name->type == ST_IDENTIFIER) {
            auto* ident = static_cast<Ad_AST_Identifier*>(post->name);
            Symbol* sym = symbol_table->resolve(ident->value);
            if (sym != nullptr && sym->scope == SymbolScope::CLASS) {
                compile_postfix_field_increment(ident->value, sym->index, post->_operator);
            } else if (sym != nullptr && sym->scope != SymbolScope::CLASS) {
                if (sym->scope == SymbolScope::FREE) {
                    std::cerr << "[ Compiler Error ] postfix ++/-- for captured variables is not supported yet\n";
                    return;
                }
                load_symbol(*sym, ident->value);
                load_symbol(*sym, ident->value);
                int one_idx = addConstant(new Ad_Integer_Object(1));
                emit(opConstant, 1, {one_idx});
                if (post->_operator == "++") {
                    emit(opAdd, 0, {});
                } else if (post->_operator == "--") {
                    emit(opSub, 0, {});
                } else {
                    std::cerr << "[ Compiler Error ] unsupported postfix increment operator\n";
                    return;
                }
                if (sym->scope == SymbolScope::GLOBAL) {
                    emit(opSetGlobal, 1, {sym->index});
                } else if (sym->scope == SymbolScope::LOCAL) {
                    emit(opSetLocal, 1, {sym->index});
                } else {
                    std::cerr << "[ Compiler Error ] postfix ++/-- for this symbol scope is not supported in the VM\n";
                    return;
                }
            } else if (symbol_table->outer != nullptr) {
                compile_postfix_field_increment(ident->value, 65535, post->_operator);
            } else {
                std::cerr << "[ Compiler Error ] undefined identifier in postfix ++/--: " << ident->value << "\n";
                return;
            }
        } else {
            std::cerr << "[ Compiler Error ] postfix ++/-- only supported for simple identifiers and index expressions in the VM\n";
            return;
        }
    } else if (node->type == ST_INFIX_EXPRESSION) {
        Ad_AST_InfixExpression* infix_expr = (Ad_AST_InfixExpression*)node;
        if (infix_expr->_operator == "<") {
            compile(infix_expr->right);
            compile(infix_expr->left);
            emit(opGreaterThan, 0, {});
            return;
        }
        if (infix_expr->_operator == "<=") {
            compile(infix_expr->right);
            compile(infix_expr->left);
            emit(opGreaterThanEqual, 0, {});
            return;
        }

        compile(infix_expr->left);
        compile(infix_expr->right);
        if (infix_expr->_operator == "+") {
            emit(opAdd, 0, {});
        } else if (infix_expr->_operator == "-") {
            emit(opSub, 0, {});
        } else if (infix_expr->_operator == "*") {
            emit(opMultiply, 0, {});
        } else if (infix_expr->_operator == "/") {
            emit(opDivide, 0, {});
        } else if (infix_expr->_operator == "%") {
            emit(opMod, 0, {});
        } else if (infix_expr->_operator == "==") {
            emit(opEqual, 0, {});
        } else if (infix_expr->_operator == "!=") {
            emit(opNotEqual, 0, {});
        } else if (infix_expr->_operator == "and" || infix_expr->_operator == "&&") {
            emit(OpCode(OP_AND), 0, {});
        } else if (infix_expr->_operator == "or" || infix_expr->_operator == "||") {
            emit(OpCode(OP_OR), 0, {});
        } else if (infix_expr->_operator == ">") {
            emit(opGreaterThan, 0, {});
        } else if (infix_expr->_operator == ">=") {
            emit(opGreaterThanEqual, 0, {});
        }
    } else if (node->type == ST_INTEGER) {
        Ad_AST_Integer* integer_node = (Ad_AST_Integer*)node;
        Ad_Integer_Object* integer_obj = new Ad_Integer_Object(integer_node->value);
        int const_index = addConstant(integer_obj);
        emit(opConstant, 1, {const_index});
    } else if (node->type == ST_FLOAT) {
        Ad_AST_Float* float_node = static_cast<Ad_AST_Float*>(node);
        Ad_Float_Object* float_obj = new Ad_Float_Object(float_node->value);
        int const_index = addConstant(float_obj);
        emit(opConstant, 1, {const_index});
    } else if (node->type == ST_BOOLEAN) {
        Ad_AST_Boolean* boolean_node = (Ad_AST_Boolean*)node;
        if (boolean_node->value) {
            emit(opTrue, 0, {});
        } else {
            emit(opFalse, 0, {});
        }
    } else if (node->type == ST_IF_EXPRESSION) {
        Ad_AST_IfExpression* if_expr = (Ad_AST_IfExpression*)node;
        compile(if_expr->condition);
        // bogus 9999 value
        std::vector<int> args = {9999};
        int jump_not_truthy_pos = emit(opJumpNotTruthy, 1, args);

        bool saved_prog_stmt = compiling_program_direct_statement;
        compiling_program_direct_statement = false;
        compile(if_expr->consequence);
        compiling_program_direct_statement = saved_prog_stmt;

        // Match evaluator: a block body does not surface the last statement value
        // (FREE_BLOCK_STATEMENT_EVAL_STATEMENT_RESULTS). Keep the block's trailing OP_POP
        // so literals like `{ 10 }` are discarded, then push null as this branch's value.
        if (if_expr->consequence != nullptr && if_expr->consequence->type == ST_BLOCK_STATEMENT) {
            emit(opNull, 0, {});
        } else if (lastInstructionIs(opPop)) {
            removeLastPop();
        }

        // op_jump with bogus 9999 value
        args = {9999};
        int jump_pos = emit(opJump, 1, args);

        int after_consequence_pos = code.instructions.size;
        changeOperand(jump_not_truthy_pos, after_consequence_pos);

        if (if_expr->alternative == nullptr) {
            emit(opNull, 0, {});
        } else {
            saved_prog_stmt = compiling_program_direct_statement;
            compiling_program_direct_statement = false;
            compile(if_expr->alternative);
            compiling_program_direct_statement = saved_prog_stmt;

            if (if_expr->alternative != nullptr && if_expr->alternative->type == ST_BLOCK_STATEMENT) {
                emit(opNull, 0, {});
            } else if (lastInstructionIs(opPop)) {
                removeLastPop();
            }
        }

        int after_alternative_pos = code.instructions.size;
        changeOperand(jump_pos, after_alternative_pos);

        emit(opPop, 0, {}); // ATENTIE! pentru ca if nu e expresie imbricata in expression statetement, din cauza parserului, aici trebuie sa pun explitic pop()
    } else if (node->type == ST_BLOCK_STATEMENT) {
        bool saved_prog = compiling_program_direct_statement;
        compiling_program_direct_statement = false;
        Ad_AST_BlockStatement* block_stmt = (Ad_AST_BlockStatement*)node;
        for (Ad_AST_Node* stmt : block_stmt->statements) {
            compile(stmt);
        }
        compiling_program_direct_statement = saved_prog;
    } else if (node->type == ST_NULL_EXPRESSION) {
        emit(opNull, 0, {});
    } else if (node->type == ST_LET_STATEMENT) {
        Ad_AST_LetStatement* let_stmt = (Ad_AST_LetStatement*)node;
        Symbol symbol = symbol_table->define(let_stmt->name.value);
        compile(let_stmt->value);
        if (symbol.scope == SymbolScope::GLOBAL) {
            emit(opSetGlobal, 1, {symbol.index});
        } else {
            emit(opSetLocal, 1, {symbol.index});
        }
    } else if (node->type == ST_ASSIGN_STATEMENT) {
        Ad_AST_AssignStatement* assign_stmt = static_cast<Ad_AST_AssignStatement*>(node);
        if (assign_stmt->name->type == ST_MEMBER_ACCESS) {
            Ad_AST_MemberAccess* member_access = static_cast<Ad_AST_MemberAccess*>(assign_stmt->name);

            std::string member_name;
            if (member_access->member->type == ST_IDENTIFIER) {
                member_name = static_cast<Ad_AST_Identifier*>(member_access->member)->value;
            } else {
                member_name = member_access->member->TokenLiteral();
            }

            if (member_access->owner->type == ST_THIS_EXPRESSION) {
                Symbol* field_sym = resolve_class_field_symbol(member_name);
                compile(assign_stmt->value);
                Ad_String_Object* field = new Ad_String_Object(member_name);
                emit(opConstant, 1, {addConstant(field)});
                if (field_sym != nullptr) {
                    emit(opPatchPropertySym, 1, {field_sym->index});
                } else {
                    emit(opPatchPropertySym, 1, {65535});
                }
                return;
            }

            if (member_access->owner->type == ST_MEMBER_ACCESS) {
                compile_nested_member_property_assign(member_access, assign_stmt->value);
                return;
            }

            compile(member_access->owner);
            compile(assign_stmt->value);

            Ad_String_Object* field = new Ad_String_Object(member_name);
            emit(opConstant, 1, {addConstant(field)});
            emit(opSetProperty, 0, {});
        } else if (assign_stmt->name->type == ST_IDENTIFIER) {
            Ad_AST_Identifier* name_id = static_cast<Ad_AST_Identifier*>(assign_stmt->name);
            Symbol* existing = symbol_table->resolve(name_id->value);
            if (existing != nullptr) {
                compile(assign_stmt->value);
                if (existing->scope == SymbolScope::GLOBAL) {
                    emit(opSetGlobal, 1, {existing->index});
                } else if (existing->scope == SymbolScope::LOCAL) {
                    if (in_class_scope()) {
                        Ad_String_Object* field_name = new Ad_String_Object(name_id->value);
                        emit(opConstant, 1, {addConstant(field_name)});
                        emit(opPatchPropertySym, 1, {existing->index});
                    } else {
                        emit(opSetLocal, 1, {existing->index});
                    }
                } else if (existing->scope == SymbolScope::CLASS) {
                    Ad_String_Object* field_name = new Ad_String_Object(name_id->value);
                    emit(opConstant, 1, {addConstant(field_name)});
                    emit(opPatchPropertySym, 1, {existing->index});
                } else if (existing->scope == SymbolScope::FREE) {
                    std::cerr << "[ Compiler Error ] assignment to captured variables (free) is not supported yet\n";
                    return;
                } else if (existing->scope == SymbolScope::BUILTIN) {
                    std::cerr << "[ Compiler Error ] cannot assign to builtin\n";
                    return;
                } else {
                    std::cerr << "[ Compiler Error ] assignment to this symbol scope is not supported in the VM\n";
                    return;
                }
            } else {
                Symbol symbol = symbol_table->define(name_id->value, in_class_scope());
                compile(assign_stmt->value);
                if (symbol.scope == SymbolScope::GLOBAL) {
                    emit(opSetGlobal, 1, {symbol.index});
                } else if (symbol.scope == SymbolScope::CLASS || in_class_scope()) {
                    Ad_String_Object* field_name = new Ad_String_Object(name_id->value);
                    emit(opConstant, 1, {addConstant(field_name)});
                    emit(opPatchPropertySym, 1, {symbol.index});
                } else {
                    emit(opSetLocal, 1, {symbol.index});
                }
            }
        } else if (assign_stmt->name->type == ST_INDEX_EXPRESSION) {
            auto* index_expr = static_cast<Ad_AST_IndexExpression*>(assign_stmt->name);
            compile(index_expr->left);
            compile(index_expr->index);
            compile(assign_stmt->value);
            emit(opSetIndex, 0, {});
        } else {
            std::cerr << "[ Compiler Error ] Unsupported assign statement target\n";
        }
    } else if (node->type == ST_IDENTIFIER) {
        Ad_AST_Identifier* identifier_node = (Ad_AST_Identifier*)node;
        Symbol* symbol = symbol_table->resolve(identifier_node->value);
        if (symbol != nullptr) {
            load_symbol(*symbol, identifier_node->value);
        } else if (in_class_scope()) {
            Symbol field_sym = symbol_table->define(identifier_node->value, true);
            Ad_String_Object* field = new Ad_String_Object(identifier_node->value);
            int const_index = addConstant(field);
            emit(opConstant, 1, {const_index});
            emit(opGetPropertySym, 1, {field_sym.index});
        } else if (enclosed_in_class_method() || compiling_function_literal) {
            emit_dynamic_instance_field_lookup(identifier_node->value);
        } else if (symbol_table->outer != nullptr &&
                   program_assign_names.count(identifier_node->value) > 0) {
            emit_forward_global_lookup(identifier_node->value);
        } else if (symbol_table->outer != nullptr) {
            emit(opNull, 0, {});
        } else {
            Symbol symbol = symbol_table->define(identifier_node->value);
            emit(opGetGlobal, 1, {symbol.index});
        }
    } else if (node->type == ST_STRING_LITERAL) {
        Ad_AST_String* string_node = (Ad_AST_String*)node;
        Ad_String_Object* string_obj = new Ad_String_Object(string_node->value);
        int const_index = addConstant(string_obj);
        emit(opConstant, 1, {const_index});
    } else if (node->type == ST_LIST_LITERAL) {
        Ad_AST_ListLiteral* list_node = (Ad_AST_ListLiteral*)node;
        for (Ad_AST_Node* el : list_node->elements) {
            compile(el);
        }
        std::vector<int> args;
        args.push_back(list_node->elements.size());
        emit(opArray, 1, args);
    } else if (node->type == ST_HASH_LITERAL) {
        Ad_AST_HashLiteral* hash_node = (Ad_AST_HashLiteral*)node;
        std::vector<Ad_AST_Node*> keys;
        for (auto& kv : hash_node->pairs) {
            keys.push_back(kv.first);
        }
        std::sort(keys.begin(), keys.end(), [](Ad_AST_Node* a, Ad_AST_Node* b) {
            return a->ToString() < b->ToString();
        });
        for (Ad_AST_Node* k : keys) {
            compile(k);
            compile(hash_node->pairs[k]);
        }
        std::vector<int> args;
        args.push_back(static_cast<int>(hash_node->pairs.size()) * 2);
        emit(opHash, 1, args);
    } else if (node->type == ST_INDEX_EXPRESSION) {
        Ad_AST_IndexExpression* index_expr = (Ad_AST_IndexExpression*)node;
        compile(index_expr->left);
        if (index_expr->indexEnd != nullptr) {
            compile(index_expr->index);
            compile(index_expr->indexEnd);
            if (index_expr->step != nullptr) {
                compile(index_expr->step);
            } else {
                emit(opNull, 0, {});
            }
            emit(opSlice, 0, {});
        } else {
            compile(index_expr->index);
            std::vector<int> args;
            emit(opIndex, 0, args);
        }
    } else if (node->type == ST_FUNCTION_LITERAL) {
        Ad_AST_FunctionLiteral* fn_lit = (Ad_AST_FunctionLiteral*)node;
        enter_scope();
        const bool saved_function_literal = compiling_function_literal;
        compiling_function_literal = true;

        // When RHS of let (e.g. let foo = fn() {}), parser sets name so body can refer to self (recursion)
        if (!fn_lit->name.empty()) {
            symbol_table->define_function_name(fn_lit->name);
        }

        for (Ad_AST_Node* p : fn_lit->parameters) {
            Ad_AST_Identifier* param = (Ad_AST_Identifier*)p;
            symbol_table->define(param->value);
        }
        compile(fn_lit->body);
        if (lastInstructionIs(opPop)) {
            replaceLastPopWithReturn();
        }
        if (!lastInstructionIs(opReturnValue)) {
            emit(opReturn, 0, {});
        }
        std::vector<Symbol> free_symbols = symbol_table->free_symbols;
        auto local_names = collect_scope_locals();
        int num_locals = symbol_table->num_definitions;
        Instructions instructions = leave_scope();

        for (const Symbol& s : free_symbols) {
            load_symbol(s, s.name);
        }

        AdCompiledFunction* compiled_func = new AdCompiledFunction();
        compiled_func->instructions = new Instructions();
        compiled_func->instructions->bytes = instructions.bytes;
        compiled_func->instructions->size = instructions.size;
        compiled_func->num_locals = num_locals;
        compiled_func->local_names = local_names;
        compiled_func->num_parameters = static_cast<int>(fn_lit->parameters.size());
        assign_parameter_names(compiled_func, fn_lit->parameters);
        fill_default_arg_values(compiled_func, fn_lit->default_params);

        std::vector<int> args;
        args.push_back(addConstant(compiled_func));
        args.push_back(static_cast<int>(free_symbols.size()));
        emit(opClosure, 2, args);
        compiling_function_literal = saved_function_literal;
    } else if (node->type == ST_CALL_EXPRESSION) {
        Ad_AST_CallExpression* call_expr = (Ad_AST_CallExpression*)node;
        if (call_expr->function != nullptr && call_expr->function->type == ST_MEMBER_ACCESS) {
            auto* member_access = static_cast<Ad_AST_MemberAccess*>(call_expr->function);
            if (member_access->is_method) {
                std::string member_name;
                if (member_access->member->type == ST_IDENTIFIER) {
                    member_name = static_cast<Ad_AST_Identifier*>(member_access->member)->value;
                } else {
                    member_name = member_access->member->TokenLiteral();
                }
                if (member_access->owner->type == ST_SUPER_EXPRESSION) {
                    emit_super_method_call(static_cast<Ad_AST_Super_Expression*>(member_access->owner),
                                           member_name, call_expr->arguments, call_expr->kw_args);
                } else {
                    emit_instance_method_call(member_access->owner, member_name, call_expr->arguments,
                                              call_expr->kw_args);
                }
                return;
            }
        }
        compile(call_expr->function);
        for (Ad_AST_Node* argument : call_expr->arguments) {
            compile(argument);
        }
        emit_call_op(static_cast<int>(call_expr->arguments.size()), call_expr->kw_args);
    } else if (node->type == ST_RETURN_STATEMENT) {
        Ad_AST_ReturnStatement* ret_stmt = static_cast<Ad_AST_ReturnStatement*>(node);
        if (ret_stmt->value != nullptr) {
            compile(ret_stmt->value);
        } else {
            emit(opNull, 0, {});
        }
        emit(opReturnValue, 0, {});
    } else if (node->type == ST_DEF_STATEMENT) {
        // Compile `def name(...) { ... }` as a named function literal bound to a symbol,
        // equivalent to a let-bound function for VM bytecode generation.
        Ad_AST_Def_Statement* def_stmt = static_cast<Ad_AST_Def_Statement*>(node);
        if (def_stmt->name == nullptr || def_stmt->name->type != ST_IDENTIFIER) {
            std::cerr << "[ Compiler Error ] Invalid def statement name\n";
            return;
        }

        Ad_AST_Identifier* def_name = static_cast<Ad_AST_Identifier*>(def_stmt->name);
        Symbol symbol = symbol_table->define(def_name->value);

        enter_scope();
        // Define function name in local scope for self-recursion support.
        symbol_table->define_function_name(def_name->value);
        for (Ad_AST_Node* p : def_stmt->parameters) {
            Ad_AST_Identifier* param = static_cast<Ad_AST_Identifier*>(p);
            symbol_table->define(param->value);
        }
    compile(def_stmt->body);
    // Class methods discard block expression results (evaluator FREE_BLOCK_STATEMENT parity).
    if (!lastInstructionIs(opReturnValue)) {
        emit(opReturn, 0, {});
    }
        std::vector<Symbol> free_symbols = symbol_table->free_symbols;
        auto local_names = collect_scope_locals();
        int num_locals = symbol_table->num_definitions;
        Instructions inner_instructions = leave_scope();

        for (const Symbol& s : free_symbols) {
            load_symbol(s, s.name);
        }

        AdCompiledFunction* compiled_func = new AdCompiledFunction();
        compiled_func->instructions = new Instructions();
        compiled_func->instructions->bytes = inner_instructions.bytes;
        compiled_func->instructions->size = inner_instructions.size;
        compiled_func->num_locals = num_locals;
        compiled_func->local_names = local_names;
        compiled_func->num_parameters = static_cast<int>(def_stmt->parameters.size());
        assign_parameter_names(compiled_func, def_stmt->parameters);
        fill_default_arg_values(compiled_func, def_stmt->default_params);

        emit(opClosure, 2, {addConstant(compiled_func), static_cast<int>(free_symbols.size())});
        if (symbol.scope == SymbolScope::GLOBAL) {
            bootstrap_global_names.erase(def_name->value);
            emit(opSetGlobal, 1, {symbol.index});
        } else {
            emit(opSetLocal, 1, {symbol.index});
        }
    } else if (node->type == ST_WHILE_EXPRESSION) {
        Ad_AST_WhileExpression* w = static_cast<Ad_AST_WhileExpression*>(node);
        LoopCompilation lc;
        lc.is_for = false;
        lc.loop_begin_byte = code.instructions.size;
        loop_stack.push_back(std::move(lc));

        compile(w->condition);
        int jmp_not_truthy_pos = emit(opJumpNotTruthy, 1, {9999});
        bool saved_while_body = compiling_program_direct_statement;
        compiling_program_direct_statement = false;
        compile(w->consequence);
        compiling_program_direct_statement = saved_while_body;
        emit(opJump, 1, {loop_stack.back().loop_begin_byte});

        int exit_pos = code.instructions.size;
        changeOperand(jmp_not_truthy_pos, exit_pos);
        for (int br : loop_stack.back().pending_break_jumps) {
            changeOperand(br, exit_pos);
        }
        loop_stack.pop_back();
    } else if (node->type == ST_FOR_EXPRESSION) {
        Ad_AST_ForExprssion* f = static_cast<Ad_AST_ForExprssion*>(node);
        if (f->initialization != nullptr) {
            bool saved_for_init = compiling_program_direct_statement;
            compiling_program_direct_statement = false;
            compile(f->initialization);
            compiling_program_direct_statement = saved_for_init;
        }
        LoopCompilation lc;
        lc.is_for = true;
        lc.loop_begin_byte = code.instructions.size;
        loop_stack.push_back(std::move(lc));

        compile(f->condition);
        int jmp_not_truthy_pos = emit(opJumpNotTruthy, 1, {9999});
        bool saved_for_body = compiling_program_direct_statement;
        compiling_program_direct_statement = false;
        compile(f->body);
        compiling_program_direct_statement = saved_for_body;
        int continue_destination = code.instructions.size;
        bool saved_for_step = compiling_program_direct_statement;
        compiling_program_direct_statement = false;
        if (f->step != nullptr) {
            compile(f->step);
            // `EvalForExpression` discards the step result; postfix/prefix ++ leave a value on the stack.
            if (f->step->type == ST_POSTFIX_INCREMENT || f->step->type == ST_PREFIX_INCREMENT) {
                emit(opPop, 0, {});
            }
        }
        compiling_program_direct_statement = saved_for_step;
        emit(opJump, 1, {loop_stack.back().loop_begin_byte});

        int exit_pos = code.instructions.size;
        changeOperand(jmp_not_truthy_pos, exit_pos);
        for (int br : loop_stack.back().pending_break_jumps) {
            changeOperand(br, exit_pos);
        }
        for (int ct : loop_stack.back().pending_continue_jumps) {
            changeOperand(ct, continue_destination);
        }
        loop_stack.pop_back();
    } else if (node->type == ST_BREAK_STATEMENT) {
        emitLoopBreak();
    } else if (node->type == ST_CONTINUE_STATEMENT) {
        emitLoopContinue();
    } else if (node->type == ST_PLUS_EQUALS) {
        Ad_AST_Plus_Equals_Statement* stmt = static_cast<Ad_AST_Plus_Equals_Statement*>(node);
        const std::string op_lit = stmt->token.GetLiteral();
        if (stmt->name->type == ST_INDEX_EXPRESSION) {
            compile_compound_index_assign(static_cast<Ad_AST_IndexExpression*>(stmt->name),
                                          stmt->value, op_lit);
        } else if (stmt->name->type == ST_MEMBER_ACCESS) {
            auto* member_access = static_cast<Ad_AST_MemberAccess*>(stmt->name);
            std::string member_name;
            if (member_access->member->type == ST_IDENTIFIER) {
                member_name = static_cast<Ad_AST_Identifier*>(member_access->member)->value;
            } else {
                member_name = member_access->member->TokenLiteral();
            }
            if (member_access->owner->type == ST_THIS_EXPRESSION) {
                Symbol* field_sym = resolve_class_field_symbol(member_name);
                int sym_index = field_sym != nullptr ? field_sym->index : 65535;
                compile_compound_field_assign(member_name, sym_index, stmt->value, op_lit);
            } else {
                compile_compound_instance_member_assign(member_access, stmt->value, op_lit);
            }
        } else if (stmt->name->type == ST_IDENTIFIER) {
        auto* ident = static_cast<Ad_AST_Identifier*>(stmt->name);
        Symbol* sym = symbol_table->resolve(ident->value);
        if (sym == nullptr && symbol_table->outer != nullptr) {
            compile_compound_field_assign(ident->value, 65535, stmt->value, op_lit);
            return;
        }
        if (sym == nullptr) {
            std::cerr << "[ Compiler Error ] undefined identifier in +=/-=: " << ident->value << "\n";
            return;
        }
        if (sym->scope == SymbolScope::FREE) {
            std::cerr << "[ Compiler Error ] +=/-= for captured variables is not supported yet\n";
            return;
        }
        if (sym->scope == SymbolScope::CLASS) {
            compile_compound_field_assign(ident->value, sym->index, stmt->value, op_lit);
            return;
        }
        load_symbol(*sym, ident->value);
        compile(stmt->value);
        if (op_lit == "+=") {
            emit(opAdd, 0, {});
        } else if (op_lit == "-=") {
            emit(opSub, 0, {});
        } else {
            std::cerr << "[ Compiler Error ] unsupported compound assignment operator\n";
            return;
        }
        if (sym->scope == SymbolScope::GLOBAL) {
            emit(opSetGlobal, 1, {sym->index});
        } else if (sym->scope == SymbolScope::LOCAL) {
            emit(opSetLocal, 1, {sym->index});
        } else if (sym->scope == SymbolScope::BUILTIN) {
            std::cerr << "[ Compiler Error ] cannot assign to builtin\n";
            return;
        } else if (sym->scope == SymbolScope::FUNCTION) {
            std::cerr << "[ Compiler Error ] invalid +=/-= target\n";
            return;
        }
        } else {
            std::cerr << "[ Compiler Error ] +=/-= target not supported in the VM\n";
        }
    } else if (node->type == ST_COMMENT) {
        // Comments do not emit bytecode.
        return;
    } else if (node->type == ST_CLASS_STATEMENT) {
        compile_class_statement(static_cast<Ad_AST_Class*>(node));
    } else if (node->type == ST_THIS_EXPRESSION) {
        emit(opGetThis, 0, {});
    } else if (node->type == ST_MEMBER_ACCESS) {
        auto* member_access = static_cast<Ad_AST_MemberAccess*>(node);
        std::string member_name;
        if (member_access->member->type == ST_IDENTIFIER) {
            member_name = static_cast<Ad_AST_Identifier*>(member_access->member)->value;
        } else {
            member_name = member_access->member->TokenLiteral();
        }
        if (!member_access->is_method && member_access->owner->type == ST_THIS_EXPRESSION) {
            Symbol* field_sym = resolve_class_field_symbol(member_name);
            if (field_sym != nullptr) {
                load_symbol(*field_sym, member_name);
                return;
            }
        }
        if (member_access->is_method) {
            if (member_access->owner->type == ST_SUPER_EXPRESSION) {
                emit_super_method_call(static_cast<Ad_AST_Super_Expression*>(member_access->owner),
                                       member_name, member_access->arguments, member_access->kw_args);
            } else {
                emit_instance_method_call(member_access->owner, member_name, member_access->arguments,
                                          member_access->kw_args);
            }
            return;
        }
        if (member_access->owner->type == ST_SUPER_EXPRESSION) {
            Symbol* field_sym = resolve_class_field_symbol(member_name);
            if (field_sym != nullptr) {
                load_symbol(*field_sym, member_name);
                return;
            }
        }
        compile(member_access->owner);
        Ad_String_Object* field = new Ad_String_Object(member_name);
        emit(opConstant, 1, {addConstant(field)});
        emit(opGetProperty, 0, {});
    }
    // TODO: add support for other statement types
}

Bytecode Compiler::getBytecode() {
    Bytecode generatedBytecode;
    generatedBytecode.instructions = code.instructions;
    generatedBytecode.constants = constants;
    collect_global_names(generatedBytecode);
    generatedBytecode.bootstrap_global_names = bootstrap_global_names;
    return generatedBytecode;
}

void Compiler::snapshot_bootstrap_globals() {
    bootstrap_global_names.clear();
    SymbolTable* root = symbol_table;
    while (root != nullptr && root->outer != nullptr) {
        root = root->outer;
    }
    if (root == nullptr) {
        return;
    }
    for (const auto& entry : root->store) {
        if (entry.second.scope == SymbolScope::GLOBAL) {
            bootstrap_global_names.insert(entry.first);
        }
    }
}

void Compiler::seed_global_symbols(const std::vector<std::string>& names) {
    int max_index = -1;
    for (size_t i = 0; i < names.size(); ++i) {
        if (names[i].empty()) {
            continue;
        }
        Symbol sym(names[i], SymbolScope::GLOBAL, static_cast<int>(i));
        symbol_table->store[names[i]] = sym;
        max_index = static_cast<int>(i);
    }
    if (max_index >= 0) {
        symbol_table->num_definitions = max_index + 1;
    }
}

void Compiler::collect_global_names(Bytecode& bytecode) {
    SymbolTable* root = symbol_table;
    while (root != nullptr && root->outer != nullptr) {
        root = root->outer;
    }
    if (root == nullptr) {
        return;
    }
    int max_index = -1;
    for (const auto& entry : root->store) {
        if (entry.second.scope == SymbolScope::GLOBAL &&
            entry.second.index > max_index) {
            max_index = entry.second.index;
        }
    }
    if (max_index < 0) {
        return;
    }
    bytecode.global_names.assign(static_cast<size_t>(max_index + 1), "");
    for (const auto& entry : root->store) {
        if (entry.second.scope == SymbolScope::GLOBAL) {
            const int idx = entry.second.index;
            if (idx >= 0 && idx <= max_index) {
                bytecode.global_names[static_cast<size_t>(idx)] = entry.first;
            }
        }
    }
}

Definition* Compiler::lookup(OpCodeType op) {
    return code.lookup(static_cast<unsigned char>(op));
}

std::pair<int, std::vector<unsigned char>> Compiler::make(OpCode opcode, int n, std::vector<int> args) {
    return code.make(opcode, n, args);
}

//int Compiler::emit(OpCodeType op, int n, std::vector<int> args) {
int Compiler::emit(OpCode opcode, int n, std::vector<int> args) {
    if (args.empty()) {
        args = {};
    }
    
    auto result= code.make(opcode, n, args);
    //auto result = make(op, n, args);
    int size = result.first;
    std::vector<unsigned char> instruction = result.second;

    int pos = addInstruction(size, instruction);
    setLastInstruction(opcode, pos);
    return pos;
}

int Compiler::addInstruction(int size, std::vector<unsigned char> instruction) {
    int pos = code.instructions.size;
    
    for (int i = 0; i < size; i++) {
        code.instructions.add(instruction[i]);
    }
    
    return pos;
}

void Compiler::setLastInstruction(OpCode opcode, int pos) {
    // Get the previous instruction from current scope
    EmittedInstruction previous = scopes[scopeIndex].lastInstruction;

    // Create new last instruction
    EmittedInstruction last(opcode.byteCode, pos);

    // Update the current scope with previous and last instructions
    scopes[scopeIndex].previousInstruction = previous;
    scopes[scopeIndex].lastInstruction = last;
}

// Constants management implementation
int Compiler::addConstant(Ad_Object* obj) {
    if (gc != nullptr && obj != nullptr) {
        gc->addObject(obj);
    }
    constants.push_back(obj);
    return constants.size() - 1; // Return the index of the added constant
}

Ad_Object* Compiler::getConstant(int index) {
    if (index >= 0 && index < constants.size()) {
        return constants[index];
    }
    return nullptr; // Return nullptr for invalid index
}

Instructions Compiler::currentInstructions() {
    if (scopeIndex >= 0 && scopeIndex < scopes.size()) {
        return scopes[scopeIndex].instructions;
    }
    return Instructions(); // Return empty instructions if scope index is invalid
}

// Frame management implementation
Frame Compiler::currentFrame() {
    return frames[frames_index - 1];
}

void Compiler::pushFrame(Frame f) {
    if (frames_index >= static_cast<int>(frames.size())) {
        frames.push_back(f);
    } else {
        frames[frames_index] = f;
    }
    frames_index += 1;
}

Frame Compiler::popFrame() {
    frames_index -= 1;
    return frames[frames_index];
}

// Helper methods for control flow
bool Compiler::lastInstructionIs(OpCode opcode) {
    if (scopeIndex < 0 || scopeIndex >= static_cast<int>(scopes.size())) {
        return false;
    }
    return scopes[scopeIndex].lastInstruction.getOpcode() == opcode.byteCode;
}

void Compiler::removeLastPop() {
    int lastPos = scopes[scopeIndex].lastInstruction.getPosition();
    Definition* def = code.lookup(static_cast<unsigned char>(OP_POP));
    if (def == nullptr) {
        return;
    }
    
    int instructionWidth = 1; // opcode itself
    for (int i = 0; i < def->size; i++) {
        instructionWidth += def->operandWidths[i];
    }
    
    // Remove the last instruction
    for (int i = 0; i < instructionWidth; i++) {
        code.instructions.removeLast();
    }
    
    // Update last instruction to previous instruction
    scopes[scopeIndex].lastInstruction = scopes[scopeIndex].previousInstruction;
}

void Compiler::replaceLastPopWithReturn() {
    int lastPos = scopes[scopeIndex].lastInstruction.getPosition();
    if (lastPos < 0 || lastPos >= code.instructions.size) {
        return;
    }
    code.instructions.bytes[lastPos] = static_cast<unsigned char>(OP_RETURN_VALUE);
    scopes[scopeIndex].lastInstruction = EmittedInstruction(OP_RETURN_VALUE, lastPos);
}

void Compiler::changeOperand(int pos, int operand) {
    if (pos < 0 || pos >= code.instructions.size) {
        return;
    }
    
    unsigned char opcode = code.instructions.get(pos);
    Definition* def = code.lookup(opcode);
    if (def == nullptr || def->size == 0) {
        return;
    }
    
    // For jump instructions, the operand is 2 bytes wide
    // Write the operand at position + 1 (after the opcode)
    if (def->operandWidths[0] == 2) {
        if (pos + 1 < code.instructions.size && pos + 2 < code.instructions.size) {
            code.instructions.bytes[pos + 1] = static_cast<unsigned char>((operand >> 8) & 0xFF);
            code.instructions.bytes[pos + 2] = static_cast<unsigned char>(operand & 0xFF);
        }
    }
}

void Compiler::enter_scope() {
    CompilationScope scope;
    scope.instruction_start = code.instructions.size;
    scope.instructions = code.instructions;
    scope.lastInstruction = EmittedInstruction();
    scope.previousInstruction = EmittedInstruction();
    scopes.push_back(scope);
    scopeIndex++;
    symbol_table = new_enclosed_symbol_table(symbol_table);
}

void Compiler::enter_scope_class() {
    CompilationScope scope("class");
    scope.instruction_start = code.instructions.size;
    scope.instructions = code.instructions;
    scope.lastInstruction = EmittedInstruction();
    scope.previousInstruction = EmittedInstruction();
    scopes.push_back(scope);
    scopeIndex++;
    symbol_table = new_enclosed_symbol_table(symbol_table);
}

Instructions Compiler::leave_scope() {
    int start = scopes[scopeIndex].instruction_start;
    Instructions instructions;
    for (int i = start; i < code.instructions.size; i++) {
        instructions.bytes.push_back(code.instructions.bytes[i]);
    }
    instructions.size = static_cast<int>(instructions.bytes.size());

    rebase_jump_operands_in_extracted_chunk(instructions, start);

    // Remove this scope's instructions from the outer code (function body lives in constant only)
    if (start < code.instructions.size) {
        code.instructions.bytes.erase(code.instructions.bytes.begin() + start, code.instructions.bytes.end());
        code.instructions.size = static_cast<int>(code.instructions.bytes.size());
    }

    // Print instructions bytes
    //std::cout << "Instructions bytes: "; // comment this out for now, used for debugging
    //for (size_t i = 0; i < instructions.bytes.size(); i++) {
    //    std::cout << static_cast<int>(instructions.bytes[i]) << " ";
    //}
    //std::cout << std::endl;

    //std::cout << disassemble_instructions(instructions) << std::endl;

    // Remove the last scope
    if (scopes.size() > 0) {
        scopes.pop_back();
    }
    if (scopeIndex > 0) {
        scopeIndex--;
    }

    // Move to outer symbol table and free the inner one we're leaving
    if (symbol_table != nullptr) {
        SymbolTable* outer = symbol_table->outer;
        delete symbol_table;
        symbol_table = outer;
    }

    return instructions;
}

void Compiler::load_symbol(const Symbol& symbol, const std::string& field_name) {
    if (symbol.scope == SymbolScope::GLOBAL) {
        emit(opGetGlobal, 1, {symbol.index});
    } else if (symbol.scope == SymbolScope::LOCAL) {
        emit(opGetLocal, 1, {symbol.index});
    } else if (symbol.scope == SymbolScope::BUILTIN) {
        emit(opGetBuiltin, 1, {symbol.index});
    } else if (symbol.scope == SymbolScope::FREE) {
        emit(opGetFree, 1, {symbol.index});
    } else if (symbol.scope == SymbolScope::CLASS) {
        Ad_String_Object* field = new Ad_String_Object(field_name);
        std::vector<int> args;
        int const_index = addConstant(field);
        args.push_back(const_index);
        emit(opConstant, 1, args);
        emit(opGetPropertySym, 1, {symbol.index});
    } else if (symbol.scope == SymbolScope::FUNCTION) {
        emit(opCurrentClosure, 0, {});
    }
}

namespace {

Ad_AST_AssignStatement* class_attribute_as_assign(Ad_AST_Node* attr) {
    if (attr == nullptr) {
        return nullptr;
    }
    if (attr->type == ST_ASSIGN_STATEMENT) {
        return static_cast<Ad_AST_AssignStatement*>(attr);
    }
    if (attr->type == ST_EXPRESSION_STATEMENT) {
        auto* expr_stmt = static_cast<Ad_AST_ExpressionStatement*>(attr);
        if (expr_stmt->expression != nullptr && expr_stmt->expression->type == ST_ASSIGN_STATEMENT) {
            return static_cast<Ad_AST_AssignStatement*>(expr_stmt->expression);
        }
    }
    return nullptr;
}

std::string assign_field_name(Ad_AST_AssignStatement* assign_stmt) {
    if (assign_stmt == nullptr || assign_stmt->name == nullptr) {
        return "";
    }
    if (assign_stmt->name->type == ST_IDENTIFIER) {
        return static_cast<Ad_AST_Identifier*>(assign_stmt->name)->value;
    }
    return assign_stmt->name->TokenLiteral();
}

} // namespace

bool Compiler::in_class_scope() const {
    return scopeIndex >= 0 && scopeIndex < static_cast<int>(scopes.size()) &&
           scopes[scopeIndex].compilationType == "class";
}

bool Compiler::enclosed_in_class_method() const {
    if (scopeIndex <= 0 || scopeIndex >= static_cast<int>(scopes.size())) {
        return false;
    }
    return scopes[static_cast<size_t>(scopeIndex - 1)].compilationType == "class";
}

SymbolTable* Compiler::root_symbol_table() const {
    SymbolTable* root = symbol_table;
    while (root != nullptr && root->outer != nullptr) {
        root = root->outer;
    }
    return root;
}

void Compiler::emit_forward_global_lookup(const std::string& name) {
    SymbolTable* root = root_symbol_table();
    if (root == nullptr) {
        emit(opNull, 0, {});
        return;
    }
    Symbol* existing = root->resolve(name);
    Symbol slot;
    if (existing != nullptr && existing->scope == SymbolScope::GLOBAL) {
        slot = *existing;
    } else {
        slot = root->define(name);
    }
    emit(opGetGlobal, 1, {slot.index});
}

void Compiler::stash_compiled_value(const Symbol& slot) {
    if (slot.scope == SymbolScope::GLOBAL) {
        emit(opSetGlobal, 1, {slot.index});
    } else {
        emit(opSetLocal, 1, {slot.index});
    }
}

void Compiler::load_stashed_value(const Symbol& slot) {
    load_symbol(slot, slot.name);
}

void Compiler::compile_nested_member_property_assign(Ad_AST_MemberAccess* target,
                                                     Ad_AST_Node* value_expr) {
    if (target == nullptr || target->owner == nullptr || value_expr == nullptr) {
        return;
    }
    std::string member_name;
    if (target->member->type == ST_IDENTIFIER) {
        member_name = static_cast<Ad_AST_Identifier*>(target->member)->value;
    } else {
        member_name = target->member->TokenLiteral();
    }

    compile(target->owner);
    Symbol owner_tmp = symbol_table->define("__nested_owner_tmp");
    stash_compiled_value(owner_tmp);
    compile(value_expr);
    Symbol value_tmp = symbol_table->define("__nested_value_tmp");
    stash_compiled_value(value_tmp);
    load_stashed_value(owner_tmp);
    load_stashed_value(value_tmp);
    Ad_String_Object* field = new Ad_String_Object(member_name);
    emit(opConstant, 1, {addConstant(field)});
    emit(opSetProperty, 0, {});
}

void Compiler::emit_dynamic_instance_field_lookup(const std::string& field_name) {
    Ad_String_Object* field = new Ad_String_Object(field_name);
    emit(opConstant, 1, {addConstant(field)});
    emit(opGetPropertySym, 1, {65535});
}

void Compiler::emit_dynamic_instance_field_patch(const std::string& field_name) {
    Ad_String_Object* field = new Ad_String_Object(field_name);
    emit(opConstant, 1, {addConstant(field)});
    emit(opPatchPropertySym, 1, {65535});
}

void Compiler::compile_compound_index_assign(Ad_AST_IndexExpression* index_expr, Ad_AST_Node* value_expr,
                                             const std::string& op_lit) {
    compile(index_expr->left);
    compile(index_expr->index);
    emit(opIndex, 0, {});
    compile(value_expr);
    if (op_lit == "+=") {
        emit(opAdd, 0, {});
    } else if (op_lit == "-=") {
        emit(opSub, 0, {});
    } else {
        std::cerr << "[ Compiler Error ] unsupported compound assignment operator\n";
        return;
    }
    compile(index_expr->left);
    compile(index_expr->index);
    emit(opPatchIndex, 0, {});
}

void Compiler::compile_compound_field_assign(const std::string& field_name, int sym_index,
                                             Ad_AST_Node* value_expr, const std::string& op_lit) {
    if (sym_index == 65535) {
        emit_dynamic_instance_field_lookup(field_name);
    } else {
        Ad_String_Object* field = new Ad_String_Object(field_name);
        emit(opConstant, 1, {addConstant(field)});
        emit(opGetPropertySym, 1, {sym_index});
    }
    compile(value_expr);
    if (op_lit == "+=") {
        emit(opAdd, 0, {});
    } else if (op_lit == "-=") {
        emit(opSub, 0, {});
    } else {
        std::cerr << "[ Compiler Error ] unsupported compound assignment operator\n";
        return;
    }
    Ad_String_Object* field = new Ad_String_Object(field_name);
    emit(opConstant, 1, {addConstant(field)});
    emit(opPatchPropertySym, 1, {sym_index});
}

void Compiler::compile_compound_instance_member_assign(Ad_AST_MemberAccess* member_access,
                                                       Ad_AST_Node* value_expr,
                                                       const std::string& op_lit) {
    std::string member_name;
    if (member_access->member->type == ST_IDENTIFIER) {
        member_name = static_cast<Ad_AST_Identifier*>(member_access->member)->value;
    } else {
        member_name = member_access->member->TokenLiteral();
    }

    compile(member_access->owner);
    compile(member_access->owner);
    Ad_String_Object* field = new Ad_String_Object(member_name);
    emit(opConstant, 1, {addConstant(field)});
    emit(opGetProperty, 0, {});
    compile(value_expr);
    if (op_lit == "+=") {
        emit(opAdd, 0, {});
    } else if (op_lit == "-=") {
        emit(opSub, 0, {});
    } else {
        std::cerr << "[ Compiler Error ] unsupported compound assignment operator\n";
        return;
    }
    emit(opConstant, 1, {addConstant(new Ad_String_Object(member_name))});
    emit(opSetProperty, 0, {});
}

void Compiler::compile_postfix_field_increment(const std::string& field_name, int sym_index,
                                               const std::string& op) {
    if (sym_index == 65535) {
        emit_dynamic_instance_field_lookup(field_name);
        emit_dynamic_instance_field_lookup(field_name);
    } else {
        Ad_String_Object* field = new Ad_String_Object(field_name);
        emit(opConstant, 1, {addConstant(field)});
        emit(opGetPropertySym, 1, {sym_index});
        emit(opConstant, 1, {addConstant(new Ad_String_Object(field_name))});
        emit(opGetPropertySym, 1, {sym_index});
    }
    int one_idx = addConstant(new Ad_Integer_Object(1));
    emit(opConstant, 1, {one_idx});
    if (op == "++") {
        emit(opAdd, 0, {});
    } else if (op == "--") {
        emit(opSub, 0, {});
    } else {
        std::cerr << "[ Compiler Error ] unsupported postfix increment operator\n";
        return;
    }
    Ad_String_Object* field = new Ad_String_Object(field_name);
    emit(opConstant, 1, {addConstant(field)});
    emit(opPatchPropertySym, 1, {sym_index});
}

AdCompiledFunction* Compiler::compile_class_field_initializer(Ad_AST_AssignStatement* assign_stmt) {
    const std::string field_name = assign_field_name(assign_stmt);
    Symbol* field_sym = symbol_table->resolve(field_name);
    if (field_sym == nullptr) {
        std::cerr << "[ Compiler Error ] unknown class field in initializer: " << field_name << "\n";
        return nullptr;
    }

    enter_scope();
    compile(assign_stmt->value);
    Ad_String_Object* field_name_obj = new Ad_String_Object(field_name);
    emit(opConstant, 1, {addConstant(field_name_obj)});
    emit(opPatchPropertySym, 1, {field_sym->index});
    emit(opReturn, 0, {});

    auto local_names = collect_scope_locals();
    int num_locals = symbol_table->num_definitions;
    Instructions instructions = leave_scope();

    auto* compiled_func = new AdCompiledFunction();
    compiled_func->instructions = new Instructions();
    compiled_func->instructions->bytes = instructions.bytes;
    compiled_func->instructions->size = instructions.size;
    compiled_func->num_locals = num_locals;
    compiled_func->local_names = local_names;
    compiled_func->num_parameters = 0;
    return compiled_func;
}

AdClosureObject* Compiler::compile_class_method(Ad_AST_Def_Statement* def_stmt) {
    if (def_stmt->name == nullptr || def_stmt->name->type != ST_IDENTIFIER) {
        std::cerr << "[ Compiler Error ] invalid class method name\n";
        return nullptr;
    }

    enter_scope();
    for (Ad_AST_Node* p : def_stmt->parameters) {
        auto* param = static_cast<Ad_AST_Identifier*>(p);
        symbol_table->define(param->value);
    }
    compile(def_stmt->body);
    // Class methods discard block expression results (evaluator FREE_BLOCK_STATEMENT parity).
    if (!lastInstructionIs(opReturnValue)) {
        emit(opReturn, 0, {});
    }

    auto local_names = collect_scope_locals();
    int num_locals = symbol_table->num_definitions;
    Instructions instructions = leave_scope();

    auto* compiled_func = new AdCompiledFunction();
    compiled_func->instructions = new Instructions();
    compiled_func->instructions->bytes = instructions.bytes;
    compiled_func->instructions->size = instructions.size;
    compiled_func->num_locals = num_locals;
    compiled_func->local_names = local_names;
    compiled_func->num_parameters = static_cast<int>(def_stmt->parameters.size());
    compiled_func->is_class_method = true;
    assign_parameter_names(compiled_func, def_stmt->parameters);
    fill_default_arg_values(compiled_func, def_stmt->default_params);

    auto* closure = new AdClosureObject();
    closure->fn = compiled_func;
    return closure;
}

Symbol* Compiler::resolve_class_field_symbol(const std::string& name) const {
    for (SymbolTable* table = symbol_table; table != nullptr; table = table->outer) {
        auto it = table->store.find(name);
        if (it != table->store.end() && it->second.scope == SymbolScope::CLASS) {
            return const_cast<Symbol*>(&it->second);
        }
    }
    return nullptr;
}

void Compiler::fill_default_arg_values(AdCompiledFunction* fn, const std::vector<Ad_AST_Node*>& default_params) {
    if (fn == nullptr) {
        return;
    }
    for (Ad_AST_Node* node : default_params) {
        Ad_Object* value = &NULLOBJECT;
        if (node == nullptr) {
            value = &NULLOBJECT;
        } else if (node->type == ST_INTEGER) {
            value = new Ad_Integer_Object(static_cast<Ad_AST_Integer*>(node)->value);
        } else if (node->type == ST_FLOAT) {
            auto* float_obj = new Ad_Float_Object();
            float_obj->value = static_cast<Ad_AST_Float*>(node)->value;
            value = float_obj;
        } else if (node->type == ST_STRING_LITERAL) {
            value = new Ad_String_Object(static_cast<Ad_AST_String*>(node)->value);
        } else if (node->type == ST_NULL_EXPRESSION) {
            value = &NULLOBJECT;
        } else if (node->type == ST_BOOLEAN) {
            value = new Ad_Boolean_Object(static_cast<Ad_AST_Boolean*>(node)->value);
        } else {
            AdClosureObject* closure = compile_default_param_closure(node);
            value = closure != nullptr ? static_cast<Ad_Object*>(closure) : &NULLOBJECT;
        }
        if (value != &NULLOBJECT && gc != nullptr) {
            gc->addObject(value);
        }
        fn->default_arg_values.push_back(value);
    }
}

AdClosureObject* Compiler::compile_default_param_closure(Ad_AST_Node* node) {
    if (node == nullptr) {
        return nullptr;
    }
    enter_scope();
    compile(node);
    if (lastInstructionIs(opPop)) {
        replaceLastPopWithReturn();
    }
    if (!lastInstructionIs(opReturnValue)) {
        emit(opReturnValue, 0, {});
    }
    if (!symbol_table->free_symbols.empty()) {
        std::cerr << "[ Compiler Warning ] default parameter free variables not fully supported in the VM\n";
    }
    auto local_names = collect_scope_locals();
    int num_locals = symbol_table->num_definitions;
    Instructions inner_instructions = leave_scope();

    auto* compiled_func = new AdCompiledFunction();
    compiled_func->instructions = new Instructions();
    compiled_func->instructions->bytes = inner_instructions.bytes;
    compiled_func->instructions->size = inner_instructions.size;
    compiled_func->num_locals = num_locals;
    compiled_func->local_names = local_names;
    compiled_func->num_parameters = 0;
    if (gc != nullptr) {
        gc->addObject(compiled_func);
    }

    auto* closure = new AdClosureObject();
    closure->fn = compiled_func;
    closure->owns_fn = false;
    if (gc != nullptr) {
        gc->addObject(closure);
    }
    return closure;
}

void Compiler::assign_parameter_names(AdCompiledFunction* fn, const std::vector<Ad_AST_Node*>& parameters) {
    if (fn == nullptr) {
        return;
    }
    fn->parameter_names.clear();
    fn->parameter_names.reserve(parameters.size());
    for (Ad_AST_Node* param : parameters) {
        if (param == nullptr) {
            continue;
        }
        if (param->type == ST_IDENTIFIER) {
            fn->parameter_names.push_back(static_cast<Ad_AST_Identifier*>(param)->value);
        } else {
            fn->parameter_names.push_back(param->TokenLiteral());
        }
    }
}

void Compiler::emit_kw_arg_pairs(const std::vector<Ad_AST_Node*>& kw_args) {
    for (Ad_AST_Node* kw_node : kw_args) {
        if (kw_node == nullptr || kw_node->type != ST_ASSIGN_STATEMENT) {
            continue;
        }
        auto* assign = static_cast<Ad_AST_AssignStatement*>(kw_node);
        compile(assign->value);
        std::string name = assign->name != nullptr ? assign->name->TokenLiteral() : "";
        emit(opConstant, 1, {addConstant(new Ad_String_Object(name))});
    }
}

void Compiler::emit_call_op(int num_pos, const std::vector<Ad_AST_Node*>& kw_args) {
    const int num_kw = static_cast<int>(kw_args.size());
    if (num_kw == 0) {
        emit(opCall, 1, {num_pos});
    } else {
        emit_kw_arg_pairs(kw_args);
        emit(opCallKw, 2, {num_pos, num_kw});
    }
}

void Compiler::emit_instance_method_call(Ad_AST_Node* owner, const std::string& method_name,
                                         const std::vector<Ad_AST_Node*>& arguments,
                                         const std::vector<Ad_AST_Node*>& kw_args) {
    compile(owner);
    Ad_String_Object* method_name_obj = new Ad_String_Object(method_name);
    emit(opConstant, 1, {addConstant(method_name_obj)});
    emit(opGetMethod, 0, {});
    for (Ad_AST_Node* argument : arguments) {
        compile(argument);
    }
    emit_call_op(static_cast<int>(arguments.size()), kw_args);
}

void Compiler::emit_super_method_call(Ad_AST_Super_Expression* super_expr, const std::string& method_name,
                                      const std::vector<Ad_AST_Node*>& arguments,
                                      const std::vector<Ad_AST_Node*>& kw_args) {
    if (super_expr == nullptr || super_expr->target == nullptr) {
        std::cerr << "[ Compiler Error ] invalid super expression\n";
        return;
    }
    const std::string parent_name = super_expr->target->TokenLiteral();
    emit(opGetThis, 0, {});
    Ad_String_Object* parent_name_obj = new Ad_String_Object(parent_name);
    emit(opConstant, 1, {addConstant(parent_name_obj)});
    Ad_String_Object* method_name_obj = new Ad_String_Object(method_name);
    emit(opConstant, 1, {addConstant(method_name_obj)});
    emit(opGetSuperMethod, 0, {});
    for (Ad_AST_Node* argument : arguments) {
        compile(argument);
    }
    emit_call_op(static_cast<int>(arguments.size()), kw_args);
}

void Compiler::merge_parent_class(AdCompiledClass* klass, AdCompiledClass* parent,
                                  const std::string& parent_name) {
    if (klass == nullptr || parent == nullptr) {
        return;
    }
    klass->supers.push_back(parent);
    klass->super_classes_by_name[parent_name] = parent;

    for (const auto& entry : parent->field_name_to_index) {
        if (klass->field_name_to_index.find(entry.first) == klass->field_name_to_index.end()) {
            Symbol field_sym = symbol_table->define(entry.first, true);
            klass->field_name_to_index[entry.first] = field_sym.index;
        }
    }
}

void Compiler::compile_class_statement(Ad_AST_Class* class_node) {
    if (class_node == nullptr || class_node->name == nullptr ||
        class_node->name->type != ST_IDENTIFIER) {
        std::cerr << "[ Compiler Error ] invalid class statement\n";
        return;
    }

    auto* class_ident = static_cast<Ad_AST_Identifier*>(class_node->name);
    Symbol class_sym = symbol_table->define(class_ident->value);

    auto* klass = new AdCompiledClass();
    klass->name = class_ident->value;
    enter_scope_class();

    for (Ad_AST_Node* parent_node : class_node->inheritFrom) {
        if (parent_node == nullptr) {
            continue;
        }
        const std::string parent_name = parent_node->TokenLiteral();
        auto parent_it = compiled_classes.find(parent_name);
        if (parent_it == compiled_classes.end() || parent_it->second == nullptr) {
            std::cerr << "[ Compiler Error ] unknown parent class: " << parent_name << "\n";
            continue;
        }
        merge_parent_class(klass, parent_it->second, parent_name);
    }

    for (Ad_AST_Node* attr : class_node->attributes) {
        Ad_AST_AssignStatement* assign_stmt = class_attribute_as_assign(attr);
        if (assign_stmt == nullptr) {
            continue;
        }
        const std::string field_name = assign_field_name(assign_stmt);
        if (field_name.empty()) {
            continue;
        }
        Symbol field_sym = symbol_table->define(field_name, true);
        klass->field_name_to_index[field_name] = field_sym.index;
    }

    for (Ad_AST_Node* attr : class_node->attributes) {
        Ad_AST_AssignStatement* assign_stmt = class_attribute_as_assign(attr);
        if (assign_stmt == nullptr) {
            continue;
        }
        AdCompiledFunction* initializer = compile_class_field_initializer(assign_stmt);
        if (initializer != nullptr) {
            klass->field_initializers.push_back(initializer);
        }
    }

    for (Ad_AST_Node* method_node : class_node->methods) {
        if (method_node == nullptr || method_node->type != ST_DEF_STATEMENT) {
            continue;
        }
        auto* def_stmt = static_cast<Ad_AST_Def_Statement*>(method_node);
        if (def_stmt->name == nullptr || def_stmt->name->type != ST_IDENTIFIER) {
            continue;
        }
        auto* method_ident = static_cast<Ad_AST_Identifier*>(def_stmt->name);
        AdClosureObject* method_closure = compile_class_method(def_stmt);
        if (method_closure != nullptr) {
            klass->methods[method_ident->value] = method_closure;
        }
    }

    leave_scope();

    compiled_classes[klass->name] = klass;

    emit(opConstant, 1, {addConstant(klass)});
    if (class_sym.scope == SymbolScope::GLOBAL) {
        emit(opSetGlobal, 1, {class_sym.index});
    } else {
        emit(opSetLocal, 1, {class_sym.index});
    }
}

std::vector<std::pair<std::string, int>> Compiler::collect_scope_locals() const {
    std::vector<std::pair<std::string, int>> locals;
    if (symbol_table == nullptr) {
        return locals;
    }
    for (const auto& entry : symbol_table->store) {
        if (entry.second.scope == SymbolScope::LOCAL) {
            locals.emplace_back(entry.first, entry.second.index);
        }
    }
    std::sort(locals.begin(), locals.end(),
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                  return a.second < b.second;
              });
    return locals;
}

