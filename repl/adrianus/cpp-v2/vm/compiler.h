#ifndef AD_VM_COMPILER_H
#define AD_VM_COMPILER_H

#include "bytecode.h"
#include "code.h"
#include "opcode.h"
#include "compilation_scope.h"
#include "frame.h"
#include "symbol_table.h"
#include "../objects.h"
#include "../ast.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Compiler {
public:
    Code code;
    GarbageCollector *gc;
    std::vector<Ad_Object*> constants;
    Bytecode bytecode;
    Instructions instructions;
    std::vector<CompilationScope> scopes;
    int scopeIndex;
    std::vector<Frame> frames;
    int frames_index;
    SymbolTable* symbol_table;

    /// Active loops for patching break/continue jumps (innermost at back).
    struct LoopCompilation {
        int loop_begin_byte{0};
        bool is_for{false};
        std::vector<int> pending_break_jumps;
        std::vector<int> pending_continue_jumps;
    };
    std::vector<LoopCompilation> loop_stack;

    /// True only while compiling a direct child of `ST_PROGRAM` (top-level script statements).
    bool compiling_program_direct_statement = false;

    /// Classes compiled so far in the current compilation unit (for inheritance resolution).
    std::unordered_map<std::string, AdCompiledClass*> compiled_classes;

    /// Global names registered while compiling bootstrap (excluded from VM `__locals()`).
    std::unordered_set<std::string> bootstrap_global_names;

    void emitLoopBreak();
    void emitLoopContinue();

    Compiler();
    Compiler(GarbageCollector* gc);
    ~Compiler();
    void reset();
    void compile(Ad_AST_Node* node);
    Bytecode getBytecode();
    void collect_global_names(Bytecode& bytecode);
    void snapshot_bootstrap_globals();
    
    // New methods for instruction emission
    int emit(OpCode op, int n = 0, std::vector<int> args = {});
    Definition* lookup(OpCodeType op);
    std::pair<int, std::vector<unsigned char>> make(OpCode opcode, int n, std::vector<int> args);
    int addInstruction(int size, std::vector<unsigned char> instruction);
    void setLastInstruction(OpCode op, int pos);
    
    // Constants management
    int addConstant(Ad_Object* obj);
    Ad_Object* getConstant(int index);
    
    // Scope management
    Instructions currentInstructions();
    void enter_scope();
    void enter_scope_class();
    Instructions leave_scope();

    // Frame management
    Frame currentFrame();
    void pushFrame(Frame f);
    Frame popFrame();

    // Helper methods for control flow
    bool lastInstructionIs(OpCode opcode);
    void removeLastPop();
    void replaceLastPopWithReturn();
    void changeOperand(int pos, int operand);

    // Symbol loading
    void load_symbol(const Symbol& symbol, const std::string& field_name = "");

    bool in_class_scope() const;
    void compile_class_statement(Ad_AST_Class* class_node);
    void merge_parent_class(AdCompiledClass* klass, AdCompiledClass* parent, const std::string& parent_name);
    void emit_instance_method_call(Ad_AST_Node* owner, const std::string& method_name,
                                   const std::vector<Ad_AST_Node*>& arguments,
                                   const std::vector<Ad_AST_Node*>& kw_args = {});
    void emit_super_method_call(Ad_AST_Super_Expression* super_expr, const std::string& method_name,
                                const std::vector<Ad_AST_Node*>& arguments,
                                const std::vector<Ad_AST_Node*>& kw_args = {});
    void emit_kw_arg_pairs(const std::vector<Ad_AST_Node*>& kw_args);
    void emit_call_op(int num_pos, const std::vector<Ad_AST_Node*>& kw_args);
    void assign_parameter_names(AdCompiledFunction* fn, const std::vector<Ad_AST_Node*>& parameters);
    AdCompiledFunction* compile_class_field_initializer(Ad_AST_AssignStatement* assign_stmt);
    AdClosureObject* compile_class_method(Ad_AST_Def_Statement* def_stmt);
    AdClosureObject* compile_default_param_closure(Ad_AST_Node* node);
    void fill_default_arg_values(AdCompiledFunction* fn, const std::vector<Ad_AST_Node*>& default_params);
    std::vector<std::pair<std::string, int>> collect_scope_locals() const;
    Symbol* resolve_class_field_symbol(const std::string& name) const;
    void emit_dynamic_instance_field_lookup(const std::string& field_name);
    void emit_dynamic_instance_field_patch(const std::string& field_name);
    void compile_compound_index_assign(Ad_AST_IndexExpression* index_expr, Ad_AST_Node* value_expr,
                                       const std::string& op_lit);
    void compile_compound_field_assign(const std::string& field_name, int sym_index,
                                       Ad_AST_Node* value_expr, const std::string& op_lit);
    void compile_compound_instance_member_assign(Ad_AST_MemberAccess* member_access,
                                                 Ad_AST_Node* value_expr,
                                                 const std::string& op_lit);
    void compile_postfix_field_increment(const std::string& field_name, int sym_index,
                                         const std::string& op);

};

#endif