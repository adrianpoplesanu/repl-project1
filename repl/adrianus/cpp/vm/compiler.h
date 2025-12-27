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

    Compiler();
    Compiler(GarbageCollector* gc);
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
    void changeOperand(int pos, int operand);

};

#endif