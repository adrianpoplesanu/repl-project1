#ifndef __BYTECODE_H
#define __BYTECODE_H

#include "instructions.h"
#include "../objects.h"

class Bytecode {
public:
    Instructions instructions;
    std::vector<Ad_Object*> constants;
};

#endif