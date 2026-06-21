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

    void emitLoopBreak();
    void emitLoopContinue();

    Compiler();
    Compiler(GarbageCollector* gc);
    ~Compiler();
    void reset();
    void compile(Ad_AST_Node* node);
    Bytecode getBytecode();
    
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

};

#endif