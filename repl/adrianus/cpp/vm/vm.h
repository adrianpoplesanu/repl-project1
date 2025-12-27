#ifndef AD_VM_H
#define AD_VM_H

#include "bytecode.h"
#include "frame.h"
#include "opcode.h"
#include "../objects.h"
#include "../gc.h"
#include <vector>

class VM {
public:
    int stackSize = 2048;
    int sp; // stack pointer
    Ad_Object *stack[2048];
    GarbageCollector *gc;
    std::vector<Frame> frames;
    std::vector<Ad_Object*> constants;
    std::vector<Ad_Object*> globals;

    VM();
    void load(Bytecode bytecode);
    void run();
    Ad_Object* last_popped_stack_element();
    Frame* current_frame();
    void push(Ad_Object* obj);
    Ad_Object* pop();
    void execute_binary_operation(OpCodeType opcode);
    Ad_Object* native_bool_to_boolean_object(bool value);
    bool is_truthy(Ad_Object* obj);
};

#endif