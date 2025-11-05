#ifndef AD_VM_FRAME_H
#define AD_VM_FRAME_H

#include "objects.h"
#include "instructions.h"

class Frame {
public:
    AdClosureObject* cl;
    int ip;
    int base_pointer;
    AdCompiledInstance* bound_instance;

    Frame(AdClosureObject* cl, int ip, int base_pointer = 0, AdCompiledInstance* bound_instance = nullptr);
    Instructions* instructions();
};

#endif