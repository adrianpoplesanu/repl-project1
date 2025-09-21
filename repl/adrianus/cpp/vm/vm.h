#ifndef AD_VM_H
#define AD_VM_H

class VM {
public:
    int stackSize = 2048;
    int sp; // stack pointer
    Ad_Object *stack[2048];
    GarbageCollector *gc;

    VM();
    void load(Bytecode bytecode);
    void run();
    Ad_Object* last_popped_stack_element();
};

#endif