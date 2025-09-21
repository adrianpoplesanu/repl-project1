#ifndef AD_VM_BYTECODE_H
#define AD_VM_BYTECODE_H

#include "instructions.h"
#include "../objects.h"

class Bytecode {
public:
    Instructions instructions;
    std::vector<Ad_Object*> constants;
};

#endif