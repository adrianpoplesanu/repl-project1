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
    int frames_index;
    std::vector<Ad_Object*> constants;
    std::vector<Ad_Object*> globals;
    Bytecode last_loaded_bytecode;
    bool has_loaded_bytecode;

    VM();
    void load(Bytecode bytecode);
    void printLogs();
    void run();
    Ad_Object* last_popped_stack_element();
    Frame* current_frame();
    void push_frame(const Frame& f);
    Frame pop_frame();
    void push(Ad_Object* obj);
    Ad_Object* pop();
    void execute_binary_operation(OpCodeType opcode);
    Ad_Object* native_bool_to_boolean_object(bool value);
    bool is_truthy(Ad_Object* obj);
    Ad_Object* build_array(int start_index, int end_index);
    Ad_Object* build_hash(int start_index, int end_index);
    void execute_index_expression(Ad_Object* left, Ad_Object* index);
    void execute_array_index(Ad_Object* left, Ad_Object* index);
    void execute_hash_index(Ad_Object* left, Ad_Object* index);

    void execute_call(int num_args);
    void call_closure(AdClosureObject* cl, int num_args);
    void call_builtin(Ad_Builtin_Object* builtin, int num_args);
    void call_class(AdCompiledClass* cl, int num_args);
    void call_bound_method(AdBoundMethod* bm, int num_args);
};

#endif