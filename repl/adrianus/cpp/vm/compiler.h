#ifndef AD_VM_COMPILER_H
#define AD_VM_COMPILER_H

#include "bytecode.h"
#include "code.h"

class Compiler {
public:
    Code code;
    GarbageCollector *gc;

    Compiler();
    void reset();
    void compile(Ad_AST_Program node);
    Bytecode getBytecode();
};

#endif