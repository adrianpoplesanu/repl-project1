#ifndef AD_VM_COMPILER_H
#define AD_VM_COMPILER_H

#include "bytecode.h"
#include "code.h"
#include "opcode.h"

class Compiler {
public:
    Code code;
    GarbageCollector *gc;

    Compiler();
    void reset();
    void compile(Ad_AST_Program node);
    Bytecode getBytecode();
    
    // New methods for instruction emission
    int emit(OpCodeType op, int n = 0, std::vector<int> args = {});
    Definition* lookup(OpCodeType op);
    std::pair<int, std::vector<unsigned char>> make(OpCodeType op, int n, std::vector<int> args);
    int addInstruction(int size, std::vector<unsigned char> instruction);
    void setLastInstruction(OpCodeType op, int pos);

};

#endif