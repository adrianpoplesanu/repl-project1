#include "compiler.h"
#include "opcode.h"
#include "utils.h"
#include <cstdarg>
#include <iostream>

Compiler::Compiler() {
    constants.clear();
    gc = nullptr;
    CompilationScope main_scope(code.instructions);
    scopes = {main_scope};
    scopeIndex = 0;
    frames.clear();
    frames_index = 0;
    symbol_table = new_symbol_table();
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
    // bytecode, instructions, and scopes are initialized by their default constructors
}

void Compiler::reset() {
    instructions = Instructions();
    bytecode = Bytecode();
}

void Compiler::compile(Ad_AST_Node* node) {
    if (node == nullptr) {
        std::cout << "severe error: node is null" << std::endl;
    } else if (node->type == ST_PROGRAM) {
        Ad_AST_Program* program = (Ad_AST_Program*)node;
        for (Ad_AST_Node* stmt : program->statements) {
            compile(stmt);
        }
    } else if (node->type == ST_EXPRESSION_STATEMENT) {
        Ad_AST_ExpressionStatement* expr_stmt = (Ad_AST_ExpressionStatement*)node;
        if (expr_stmt->expression != nullptr) {
            if (expr_stmt->expression->type == ST_DEF_STATEMENT) {
                // hmmm, dupa multe cautari am gasit fixul asta, ce ciudat mi se pare
                compile(expr_stmt->expression);
            } else if (expr_stmt->expression->type == ST_ASSIGN_STATEMENT) {
                compile(expr_stmt->expression);
            } else if (expr_stmt->expression->type == ST_WHILE_EXPRESSION) {
                compile(expr_stmt->expression);
            } else {
                compile(expr_stmt->expression);
                emit(opPop, 0, {});
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
        } else if (infix_expr->_operator == "==") {
            emit(opEqual, 0, {});
        } else if (infix_expr->_operator == "!=") {
            emit(opNotEqual, 0, {});
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

        compile(if_expr->consequence);

        if (lastInstructionIs(opPop)) {
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
            compile(if_expr->alternative);

            if (lastInstructionIs(opPop)) {
                removeLastPop();
            }
        }

        int after_alternative_pos = code.instructions.size;
        changeOperand(jump_pos, after_alternative_pos);

        emit(opPop, 0, {}); // ATENTIE! pentru ca if nu e expresie imbricata in expression statetement, din cauza parserului, aici trebuie sa pun explitic pop()
    } else if (node->type == ST_BLOCK_STATEMENT) {
        Ad_AST_BlockStatement* block_stmt = (Ad_AST_BlockStatement*)node;
        for (Ad_AST_Node* stmt : block_stmt->statements) {
            compile(stmt);
        }
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
    } else if (node->type == ST_IDENTIFIER) {
        Ad_AST_Identifier* identifier_node = (Ad_AST_Identifier*)node;
        Symbol* symbol = symbol_table->resolve(identifier_node->value);
        if (symbol != nullptr) {
            load_symbol(*symbol, identifier_node->value);
        } else {
            // Handle implicit 'this' property access
            Ad_String_Object* field = new Ad_String_Object(identifier_node->value);
            std::vector<int> args;
            int const_index = addConstant(field);
            args.push_back(const_index);
            emit(opConstant, 1, args);
            emit(opGetPropertySym, 1, {0});
        }
    } else if (node->type == ST_STRING_LITERAL) {
        Ad_AST_String* string_node = (Ad_AST_String*)node;
        Ad_String_Object* string_obj = new Ad_String_Object(string_node->value);
        int const_index = addConstant(string_obj);
        emit(opConstant, 1, {const_index});
    }
    // TODO: add support for other statement types
}

Bytecode Compiler::getBytecode() {
    Bytecode generatedBytecode;
    generatedBytecode.instructions = code.instructions;
    generatedBytecode.constants = constants;
    return generatedBytecode;
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
    scope.instructions = code.instructions;
    scope.lastInstruction = EmittedInstruction();
    scope.previousInstruction = EmittedInstruction();
    scopes.push_back(scope);
    scopeIndex++;
    symbol_table = new_enclosed_symbol_table(symbol_table);
}

void Compiler::enter_scope_class() {
    CompilationScope scope("class");
    scope.instructions = code.instructions;
    scope.lastInstruction = EmittedInstruction();
    scope.previousInstruction = EmittedInstruction();
    scopes.push_back(scope);
    scopeIndex++;
    symbol_table = new_enclosed_symbol_table(symbol_table);
}

Instructions Compiler::leave_scope() {
    Instructions instructions = currentInstructions();
    
    // Print instructions bytes
    std::cout << "Instructions bytes: ";
    for (size_t i = 0; i < instructions.bytes.size(); i++) {
        std::cout << static_cast<int>(instructions.bytes[i]) << " ";
    }
    std::cout << std::endl;
    
    // Disassemble instructions
    /*Code temp_code;
    temp_code.instructions = instructions;
    std::string disassembled = temp_code.toString();
    std::cout << disassembled << std::endl;*/
    std::cout << disassemble_instructions(instructions) << std::endl;
    
    // Remove the last scope
    if (scopes.size() > 0) {
        scopes.pop_back();
    }
    if (scopeIndex > 0) {
        scopeIndex--;
    }
    
    // Move to outer symbol table
    if (symbol_table != nullptr) {
        symbol_table = symbol_table->outer;
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

