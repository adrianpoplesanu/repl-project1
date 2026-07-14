#ifndef AD_VM_H
#define AD_VM_H

#include "bytecode.h"
#include "frame.h"
#include "opcode.h"
#include "../objects.h"
#include "../gc.h"
#include <vector>
#include <unordered_set>
#include <string>

class Environment;

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
    std::vector<std::string> global_names;
    std::unordered_set<std::string> bootstrap_global_names;
    Bytecode last_loaded_bytecode;
    bool has_loaded_bytecode;
    /// When true, prints "WARNING: return outside function" when returning from top-level frame.
    bool warn_return_outside_function;

    VM();
    void load(Bytecode bytecode);
    void printLogs();
    /// Run until completion. If `max_instructions` is 0, no instruction limit is enforced.
    void run(uint64_t max_instructions = 0);
    /// Run instructions until `frames_index` is at most `target_frames_index`.
    void run_until_frames_index(int target_frames_index, uint64_t max_instructions = 0);
    bool execute_instruction();
    Ad_Object* last_popped_stack_element();
    Frame* current_frame();
    void push_frame(const Frame& f);
    Frame pop_frame();
    void push(Ad_Object* obj);
    Ad_Object* pop();
    void execute_binary_operation(OpCodeType opcode);
    void execute_comparison(OpCodeType opcode);
    void execute_bang_operator();
    void execute_minus_operator();
    Ad_Object* native_bool_to_boolean_object(bool value);
    bool is_truthy(Ad_Object* obj);
    Ad_Object* build_array(int start_index, int end_index);
    Ad_Object* build_hash(int start_index, int end_index);
    void execute_index_expression(Ad_Object* left, Ad_Object* index);
    void execute_set_index_expression(Ad_Object* left, Ad_Object* index, Ad_Object* value);
    void execute_array_index(Ad_Object* left, Ad_Object* index);
    void execute_hash_index(Ad_Object* left, Ad_Object* index);
    void execute_string_index(Ad_Object* left, Ad_Object* index);
    void execute_slice_expression(Ad_Object* left, Ad_Object* start, Ad_Object* end, Ad_Object* step);

    void execute_call(int num_args);
    void call_closure(AdClosureObject* cl, int num_args);
    void call_builtin(Ad_Builtin_Object* builtin, int num_args);
    void call_class(AdCompiledClass* cl, int num_args);
    void call_bound_method(AdBoundMethod* bm, int num_args);
    void call_runtime_bound_method(AdRuntimeBoundMethod* bm, int num_args);
    void apply_default_arguments(AdCompiledFunction* fn, int& num_args);
    void execute_call_kw(int num_pos, int num_kw);

    Ad_Object* build_locals_hash();

    AdCompiledInstance* current_bound_instance();
    void ensure_instance_field_capacity(AdCompiledInstance* inst, int index);
    void register_instance_field_name(AdCompiledInstance* inst, const std::string& name, int index);
    int lookup_instance_field_index(AdCompiledInstance* inst, const std::string& name);
    int find_or_add_global_index(const std::string& name);
    void push_bound_instance_member(AdCompiledInstance* inst, const std::string& name);
    void execute_get_property_sym(int sym_index);
    void execute_patch_property_sym(int sym_index);
    void execute_get_property();
    void execute_set_property();
    void execute_get_method();
    void execute_get_super_method();
    void execute_get_this();
    void execute_set_method();

    Environment* create_eval_environment(GarbageCollector* gc);
    void sync_globals_from_environment(Environment* env);
    void set_instance_attribute(AdCompiledInstance* inst, const std::string& name, Ad_Object* value);
    Ad_Object* evaluate_default_arg_value(Ad_Object* default_value);

    /// Invoke a closure synchronously (used by VM thread workers).
    Ad_Object* invoke_closure(AdClosureObject* closure, const std::vector<Ad_Object*>& args);

    /// Compile and run an imported source unit, merging globals into the live VM.
    void execute_import_source(const std::string& source);
};

#endif