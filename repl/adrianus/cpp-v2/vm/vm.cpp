#include "vm.h"
#include "code.h"
#include "objects.h"
#include "utils.h"
#include "../builtins_registry.h"
#include "../evaluator.h"
#include "../hashpair.h"
#include "../utils.h"
#include "../socket_utils.h"
#include "../thread_utils.h"
#include "../environment.h"
#include <iostream>
#include <vector>
#include <functional>
#include <cstdint>
#include <unordered_map>
#include <climits>
#include <algorithm>

extern Ad_Object* locals_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector* gc);

static std::string vm_property_field_name(Ad_Object* field_name_obj);

namespace {

bool is_nullish(Ad_Object* obj) {
    return obj == nullptr || obj == &NULLOBJECT || obj->Type() == OBJ_NULL;
}

int int_value(Ad_Object* obj) {
    return static_cast<Ad_Integer_Object*>(obj)->value;
}

Ad_Object* vm_new_sub_list(Ad_Object* target, int i1, int i2, int step, GarbageCollector* gc) {
    std::vector<Ad_Object*> elements;
    int max = static_cast<int>(static_cast<Ad_List_Object*>(target)->elements.size());

    if (step < 0 && i1 < -max) {
        return new Ad_List_Object();
    }
    if (step < 0 && i1 >= max) {
        i1 = INT_MAX;
    }
    if (step < 0 && i2 < -max) {
        i2 = -INT_MAX;
    }

    if (i1 == INT_MAX) {
        i1 = max - 1;
    } else {
        if (i1 < -max) i1 = -max;
        if (i1 < 0) i1 += max;
        if (i1 > max) i1 = max;
    }

    if (i2 == -INT_MAX) {
        i2 = -1;
    } else {
        if (i2 < -max) i2 = -max;
        if (i2 < 0) i2 += max;
        if (i2 > max) i2 = max;
    }

    if (step > 0) {
        for (int i = i1; i < i2; i += step) {
            elements.push_back(static_cast<Ad_List_Object*>(target)->elements.at(static_cast<size_t>(i))->copy(gc));
        }
    } else if (step < 0) {
        for (int i = i1; i > i2; i += step) {
            elements.push_back(static_cast<Ad_List_Object*>(target)->elements.at(static_cast<size_t>(i))->copy(gc));
        }
    }
    auto* result = new Ad_List_Object(elements);
    if (gc != nullptr) {
        gc->addObject(result);
    }
    return result;
}

Ad_Object* vm_new_sub_string(Ad_Object* target, int i1, int i2, int step, GarbageCollector* gc) {
    std::string original = static_cast<Ad_String_Object*>(target)->value;
    std::string result;

    if (i1 < 0 && i2 < 0 && step > 0) {
        i1 += static_cast<int>(original.length());
        i2 += static_cast<int>(original.length());
    } else if (i1 < 0 && i2 < 0 && step < 0) {
        i1 += static_cast<int>(original.length());
        if (i2 < -static_cast<int>(original.length())) {
            i2 = -1;
        } else {
            i2 += static_cast<int>(original.length());
        }
    } else if (i1 < 0 && i2 == static_cast<int>(original.length()) && step < 0) {
        if (i1 < -static_cast<int>(original.length())) {
            return new Ad_String_Object("");
        }
        i1 += static_cast<int>(original.length());
        i2 = -1;
    } else if (i1 < 0 && i2 > 0) {
        i1 += static_cast<int>(original.length());
    } else if (i1 > 0 && i2 < 0 && step < 0) {
        i2 += static_cast<int>(original.length());
    } else if (i1 >= 0 && i2 < 0 && step > 0) {
        i2 += static_cast<int>(original.length());
    } else if (i1 < 0 && i2 >= 0 && step < 0) {
        i1 += static_cast<int>(original.length());
    } else if (i1 == 0 && i2 > 0 && step < 0) {
        i1 += static_cast<int>(original.length()) - 1;
        if (i2 == static_cast<int>(original.length()) - 1) {
            i2 = -1;
        }
    }

    if (i1 < i2 && step > 0) {
        for (int i = i1; i < i2; i += step) {
            result += original.at(static_cast<size_t>(i));
        }
    } else if (i1 > i2 && step < 0) {
        for (int i = i1; i > i2; i += step) {
            result += original.at(static_cast<size_t>(i));
        }
    } else if (i1 < i2 && step < 0) {
        for (int i = i2; i >= i1; i += step) {
            result += original.at(static_cast<size_t>(i));
        }
    }

    auto* out = new Ad_String_Object(result);
    if (gc != nullptr) {
        gc->addObject(out);
    }
    return out;
}

bool vm_objects_equal(Ad_Object* left, Ad_Object* right) {
    if (left == right) {
        return true;
    }
    if (left == nullptr || right == nullptr) {
        return false;
    }
    if (left->Type() != right->Type()) {
        return false;
    }
    switch (left->Type()) {
        case OBJ_NULL:
            return true;
        case OBJ_BOOL:
            return static_cast<Ad_Boolean_Object*>(left)->value ==
                   static_cast<Ad_Boolean_Object*>(right)->value;
        case OBJ_INT:
            return static_cast<Ad_Integer_Object*>(left)->value ==
                   static_cast<Ad_Integer_Object*>(right)->value;
        case OBJ_FLOAT:
            return static_cast<Ad_Float_Object*>(left)->value ==
                   static_cast<Ad_Float_Object*>(right)->value;
        case OBJ_STRING:
            return static_cast<Ad_String_Object*>(left)->value ==
                   static_cast<Ad_String_Object*>(right)->value;
        case OBJ_HASH: {
            const auto& left_pairs = static_cast<Ad_Hash_Object*>(left)->pairs;
            const auto& right_pairs = static_cast<Ad_Hash_Object*>(right)->pairs;
            if (left_pairs.size() != right_pairs.size()) {
                return false;
            }
            for (const auto& entry : left_pairs) {
                auto right_it = right_pairs.find(entry.first);
                if (right_it == right_pairs.end()) {
                    return false;
                }
                if (!vm_objects_equal(entry.second.key, right_it->second.key)) {
                    return false;
                }
                if (!vm_objects_equal(entry.second.value, right_it->second.value)) {
                    return false;
                }
            }
            return true;
        }
        case OBJ_LIST: {
            const auto& left_elems = static_cast<Ad_List_Object*>(left)->elements;
            const auto& right_elems = static_cast<Ad_List_Object*>(right)->elements;
            if (left_elems.size() != right_elems.size()) {
                return false;
            }
            for (size_t i = 0; i < left_elems.size(); ++i) {
                if (!vm_objects_equal(left_elems[i], right_elems[i])) {
                    return false;
                }
            }
            return true;
        }
        default:
            return false;
    }
}

} // namespace

VM::VM() {
    sp = 0;
    gc = nullptr;
    frames.clear();
    frames_index = 0;
    constants.clear();
    globals.clear();
    global_names.clear();
    has_loaded_bytecode = false;
    warn_return_outside_function = true;
}

void VM::load(Bytecode bytecode) {
    last_loaded_bytecode = bytecode;
    has_loaded_bytecode = true;

    // Store constants from bytecode
    constants = bytecode.constants;
    global_names = bytecode.global_names;
    bootstrap_global_names = bytecode.bootstrap_global_names;
    globals.clear();
    
    // Create an AdCompiledFunction with the instructions
    AdCompiledFunction* compiledFn = new AdCompiledFunction();
    Instructions* instructions = new Instructions();
    instructions->bytes = bytecode.instructions.bytes;
    instructions->size = bytecode.instructions.size;
    compiledFn->instructions = instructions;
    
    // Create an AdClosureObject wrapping the compiled function
    AdClosureObject* closure = new AdClosureObject();
    closure->fn = compiledFn;
    
    // Create the initial frame with the closure
    frames.clear();
    frames_index = 0;
    Frame frame(closure, -1, 0, nullptr);
    push_frame(frame);
}

void VM::printLogs() {
    if (!has_loaded_bytecode) {
        return;
    }
    write_bytecode_log(last_loaded_bytecode);
}

bool VM::execute_instruction() {
    Frame* frame = current_frame();
    if (frame == nullptr) {
        return false;
    }
    if (frame->ip >= static_cast<int>(frame->instructions()->bytes.size()) - 1) {
        return false;
    }

    frame->ip += 1;
    int ip = frame->ip;
    Instructions* ins = frame->instructions();

    if (ip < 0 || ip >= static_cast<int>(ins->bytes.size())) {
        return false;
    }

    OpCodeType opcode = static_cast<OpCodeType>(ins->bytes[ip]);

    if (opcode == OP_CONSTANT) {
        int const_index = read_uint16(*ins, ip + 1);
        frame->ip += 2;
        if (const_index >= 0 && const_index < static_cast<int>(constants.size())) {
            push(constants[const_index]);
        }
    } else if (opcode == OP_ADD || opcode == OP_SUB || opcode == OP_MULTIPLY || opcode == OP_DIVIDE) {
        execute_binary_operation(opcode);
    } else if (opcode == OP_EQUAL || opcode == OP_NOTEQUAL ||
               opcode == OP_GREATERTHAN || opcode == OP_GREATERTHAN_EQUAL) {
        execute_comparison(opcode);
    } else if (opcode == OP_AND || opcode == OP_OR) {
        if (sp < 2) {
            std::cerr << "[ VM Error ] Not enough elements on stack for boolean op\n";
            return false;
        }
        Ad_Object* right = pop();
        Ad_Object* left = pop();
        if (left == nullptr || right == nullptr || left->Type() != OBJ_BOOL || right->Type() != OBJ_BOOL) {
            std::cerr << "[ VM Error ] Unsupported types for boolean op\n";
            push(&NULLOBJECT);
        } else {
            bool lv = static_cast<Ad_Boolean_Object*>(left)->value;
            bool rv = static_cast<Ad_Boolean_Object*>(right)->value;
            push(native_bool_to_boolean_object(opcode == OP_AND ? (lv && rv) : (lv || rv)));
        }
    } else if (opcode == OP_BANG) {
        execute_bang_operator();
    } else if (opcode == OP_MINUS) {
        execute_minus_operator();
    } else if (opcode == OP_POP) {
        pop();
    } else if (opcode == OP_FILE_STMT_OUTPUT) {
        if (sp <= 0) {
            std::cerr << "[ VM Error ] OP_FILE_STMT_OUTPUT: stack underflow\n";
        } else {
            Ad_Object* result = pop();
            if (result != nullptr && result->Type() != OBJ_SIGNAL && result->Type() != OBJ_NULL) {
                std::cout << result->Inspect() << "\n";
            }
        }
    } else if (opcode == OP_TRUE) {
        push(native_bool_to_boolean_object(true));
    } else if (opcode == OP_FALSE) {
        push(native_bool_to_boolean_object(false));
    } else if (opcode == OP_JUMP) {
        int pos = read_uint16(*ins, ip + 1);
        frame->ip = pos - 1;
    } else if (opcode == OP_JUMP_NOT_TRUTHY) {
        int pos = read_uint16(*ins, ip + 1);
        frame->ip += 2;
        Ad_Object* condition = pop();
        if (!is_truthy(condition)) {
            frame->ip = pos - 1;
        }
    } else if (opcode == OP_NULL) {
        push(&NULLOBJECT);
    } else if (opcode == OP_SET_GLOBAL) {
        int global_index = read_uint16(*ins, ip + 1);
        frame->ip += 2;
        if (global_index >= static_cast<int>(globals.size())) {
            globals.resize(global_index + 1, nullptr);
        }
        globals[global_index] = pop();
    } else if (opcode == OP_GET_GLOBAL) {
        int global_index = read_uint16(*ins, ip + 1);
        frame->ip += 2;
        if (global_index >= 0 && global_index < static_cast<int>(globals.size())) {
            push(globals[global_index]);
        } else {
            std::cerr << "[ VM Error ] Global index out of bounds: " << global_index << std::endl;
        }
    } else if (opcode == OP_SET_LOCAL) {
        int local_index = read_uint8(*ins, ip + 1);
        frame->ip += 1;
        int slot = frame->base_pointer + local_index;
        if (slot < 0 || slot >= stackSize) {
            std::cerr << "[ VM Error ] OP_SET_LOCAL: local slot out of bounds: " << slot << std::endl;
        } else {
            stack[slot] = pop();
        }
    } else if (opcode == OP_GET_LOCAL) {
        int local_index = read_uint8(*ins, ip + 1);
        frame->ip += 1;
        int slot = frame->base_pointer + local_index;
        if (slot < 0 || slot >= stackSize) {
            std::cerr << "[ VM Error ] OP_GET_LOCAL: local slot out of bounds: " << slot << std::endl;
        } else {
            push(stack[slot]);
        }
    } else if (opcode == OP_GET_BUILTIN) {
        int builtin_index = read_uint8(*ins, ip + 1);
        frame->ip += 1;
        Ad_Object* builtin_obj = vm_get_builtin_object(builtin_index);
        if (builtin_obj != nullptr) {
            push(builtin_obj);
        } else {
            std::cerr << "[ VM Error ] OP_GET_BUILTIN: invalid index " << builtin_index << std::endl;
        }
    } else if (opcode == OP_ARRAY) {
        int numElements = read_uint16(*ins, ip + 1);
        frame->ip += 2;
        Ad_Object* array_obj = build_array(sp - numElements, sp);
        sp = sp - numElements;
        push(array_obj);
    } else if (opcode == OP_HASH) {
        int numElements = read_uint16(*ins, ip + 1);
        frame->ip += 2;
        Ad_Object* hash_obj = build_hash(sp - numElements, sp);
        sp = sp - numElements;
        push(hash_obj);
    } else if (opcode == OP_INDEX) {
        Ad_Object* index = pop();
        Ad_Object* left = pop();
        execute_index_expression(left, index);
    } else if (opcode == OP_SET_INDEX) {
        Ad_Object* value = pop();
        Ad_Object* index = pop();
        Ad_Object* left = pop();
        execute_set_index_expression(left, index, value);
    } else if (opcode == OP_POSTFIX_INDEX) {
        if (sp < 4) {
            std::cerr << "[ VM Error ] OP_POSTFIX_INDEX: stack underflow\n";
        } else {
            Ad_Object* index = pop();
            Ad_Object* left = pop();
            Ad_Object* new_value = pop();
            Ad_Object* old_value = pop();
            execute_set_index_expression(left, index, new_value);
            push(old_value != nullptr ? old_value : &NULLOBJECT);
        }
    } else if (opcode == OP_SLICE) {
        if (sp < 4) {
            std::cerr << "[ VM Error ] OP_SLICE: stack underflow\n";
        } else {
            Ad_Object* step = pop();
            Ad_Object* end = pop();
            Ad_Object* start = pop();
            Ad_Object* left = pop();
            execute_slice_expression(left, start, end, step);
        }
    } else if (opcode == OP_PATCH_INDEX) {
        if (sp < 3) {
            std::cerr << "[ VM Error ] OP_PATCH_INDEX: stack underflow\n";
        } else {
            Ad_Object* index = pop();
            Ad_Object* left = pop();
            Ad_Object* value = pop();
            execute_set_index_expression(left, index, value);
        }
    } else if (opcode == OP_CLOSURE) {
        int const_index = read_uint16(*ins, ip + 1);
        int num_free = read_uint8(*ins, ip + 3);
        frame->ip += 3;
        if (const_index < 0 || const_index >= static_cast<int>(constants.size()) ||
            constants[const_index] == nullptr ||
            constants[const_index]->Type() != OBJ_COMPILED_FUNCTION) {
            std::cerr << "[ VM Error ] OP_CLOSURE invalid compiled function constant\n";
        } else {
            auto* closure = new AdClosureObject();
            closure->fn = static_cast<AdCompiledFunction*>(constants[const_index]);
            closure->free_vars.resize(static_cast<size_t>(num_free));
            for (int i = num_free - 1; i >= 0; --i) {
                closure->free_vars[static_cast<size_t>(i)] = pop();
            }
            push(closure);
        }
    } else if (opcode == OP_GET_FREE) {
        int free_index = read_uint8(*ins, ip + 1);
        frame->ip += 1;
        if (frame->cl == nullptr || free_index < 0 ||
            free_index >= static_cast<int>(frame->cl->free_vars.size())) {
            std::cerr << "[ VM Error ] OP_GET_FREE: index out of bounds\n";
        } else {
            Ad_Object* captured = frame->cl->free_vars[static_cast<size_t>(free_index)];
            push(captured != nullptr ? captured : &NULLOBJECT);
        }
    } else if (opcode == OP_CURRENT_CLOSURE) {
        if (frame->cl == nullptr) {
            std::cerr << "[ VM Error ] OP_CURRENT_CLOSURE: no active closure\n";
        } else {
            push(frame->cl);
        }
    } else if (opcode == OP_CALL) {
        int num_args = read_uint8(*ins, ip + 1);
        frame->ip += 1;
        execute_call(num_args);
    } else if (opcode == OP_CALL_KW) {
        int num_pos = read_uint8(*ins, ip + 1);
        int num_kw = read_uint8(*ins, ip + 2);
        frame->ip += 2;
        execute_call_kw(num_pos, num_kw);
    } else if (opcode == OP_RETURN_VALUE) {
        Ad_Object* return_value = pop();
        const bool returning_from_main = frames_index == 1;
        Frame ret_frame = pop_frame();
        sp = ret_frame.base_pointer - 1;
        if (sp < 0) {
            sp = 0;
        }
        if (returning_from_main && warn_return_outside_function) {
            std::cout << "WARNING: return outside function\n";
        }
        push(return_value);
        if (frames_index == 0) {
            return false;
        }
    } else if (opcode == OP_RETURN) {
        Frame ret_frame = pop_frame();
        sp = ret_frame.base_pointer - 1;
        if (sp < 0) {
            sp = 0;
        }
        push(nullptr);
        if (frames_index == 0) {
            return false;
        }
    } else if (opcode == OP_CLASS) {
        push(new AdCompiledClass());
    } else if (opcode == OP_SET_METHOD) {
        execute_set_method();
    } else if (opcode == OP_GET_PROPERTY_SYM) {
        int sym_index = read_uint16(*ins, ip + 1);
        frame->ip += 2;
        execute_get_property_sym(sym_index);
    } else if (opcode == OP_PATCH_PROPERTY_SYM) {
        int sym_index = read_uint16(*ins, ip + 1);
        frame->ip += 2;
        execute_patch_property_sym(sym_index);
    } else if (opcode == OP_GET_PROPERTY) {
        execute_get_property();
    } else if (opcode == OP_SET_PROPERTY) {
        execute_set_property();
    } else if (opcode == OP_GET_METHOD) {
        execute_get_method();
    } else if (opcode == OP_GET_THIS) {
        execute_get_this();
    } else if (opcode == OP_GET_SUPER_METHOD) {
        execute_get_super_method();
    } else {
        std::cerr << "[ VM Error ] unknown opcode: " << static_cast<int>(opcode) << std::endl;
    }

    return frames_index > 0;
}

void VM::run_until_frames_index(int target_frames_index, uint64_t max_instructions) {
    uint64_t instructions_executed = 0;
    while (frames_index > target_frames_index) {
        if (max_instructions != 0 && ++instructions_executed > max_instructions) {
            std::cerr << "[ VM Error ] instruction budget exceeded (set AD_VM_MAX_INSTRUCTIONS to raise the cap)\n";
            break;
        }
        if (!execute_instruction()) {
            break;
        }
    }
}

void VM::run(uint64_t max_instructions) {
    uint64_t instructions_executed = 0;
    while (current_frame() != nullptr &&
           current_frame()->ip < static_cast<int>(current_frame()->instructions()->bytes.size()) - 1) {
        if (max_instructions != 0 && ++instructions_executed > max_instructions) {
            std::cerr << "[ VM Error ] instruction budget exceeded (set AD_VM_MAX_INSTRUCTIONS to raise the cap)\n";
            break;
        }
        if (!execute_instruction()) {
            break;
        }
    }
}

void VM::execute_call(int num_args) {
    int callee_index = sp - 1 - num_args;
    if (callee_index < 0) {
        std::cerr << "[ VM Error ] OP_CALL: stack underflow (callee index)\n";
        return;
    }
    Ad_Object* callee = stack[callee_index];
    if (callee == nullptr) {
        std::cerr << "[ VM Error ] SEVERE ERROR: calling error - callee is nullptr\n";
        return;
    }
    Ad_Object_Type t = callee->Type();
    if (t == OBJ_CLOSURE) {
        call_closure(static_cast<AdClosureObject*>(callee), num_args);
    } else if (t == OBJ_BUILTIN) {
        call_builtin(static_cast<Ad_Builtin_Object*>(callee), num_args);
    } else if (t == OBJ_COMPILED_CLASS) {
        call_class(static_cast<AdCompiledClass*>(callee), num_args);
    } else if (t == OBJ_BOUND_METHOD) {
        call_bound_method(static_cast<AdBoundMethod*>(callee), num_args);
    } else if (t == OBJ_RUNTIME_BOUND_METHOD) {
        call_runtime_bound_method(static_cast<AdRuntimeBoundMethod*>(callee), num_args);
    } else {
        std::cerr << "[ VM Error ] SEVERE ERROR: calling error (unsupported callee type)\n";
        sp = callee_index;
        push(&NULLOBJECT);
    }
}

void VM::apply_default_arguments(AdCompiledFunction* fn, int& num_args) {
    if (fn == nullptr) {
        return;
    }
    const int num_params = fn->num_parameters;
    if (num_args >= num_params) {
        return;
    }
    const int num_defaults = static_cast<int>(fn->default_arg_values.size());
    if (num_args + num_defaults < num_params) {
        std::cerr << "ERROR: wrong number of arguments expecting: " << num_params
                  << " got: " << num_args << std::endl;
        return;
    }
    const int threshold = num_defaults - num_params + num_args;
    for (int i = 0; i < num_defaults; ++i) {
        if (i >= threshold) {
            Ad_Object* default_value = fn->default_arg_values[static_cast<size_t>(i)];
            if (default_value != nullptr) {
                push(default_value);
            } else {
                push(&NULLOBJECT);
            }
            ++num_args;
        }
    }
}

void VM::call_closure(AdClosureObject* cl, int num_args) {
    if (cl == nullptr || cl->fn == nullptr) {
        std::cerr << "[ VM Error ] call_closure: null closure or function\n";
        return;
    }
    const int callee_index = sp - 1 - num_args;
    apply_default_arguments(cl->fn, num_args);
    if (num_args != cl->fn->num_parameters) {
        sp = callee_index;
        auto* err = new Ad_Error_Object("some error message here");
        if (gc != nullptr) {
            gc->addObject(err);
        }
        push(err);
        return;
    }
    Frame frame(cl, -1, sp - num_args, current_bound_instance());
    push_frame(frame);
    sp = frame.base_pointer + cl->fn->num_locals;
}

static AdCompiledFunction* vm_function_from_callee(Ad_Object* callee) {
    if (callee == nullptr) {
        return nullptr;
    }
    if (callee->Type() == OBJ_CLOSURE) {
        auto* cl = static_cast<AdClosureObject*>(callee);
        return cl != nullptr ? cl->fn : nullptr;
    }
    if (callee->Type() == OBJ_BOUND_METHOD) {
        auto* bm = static_cast<AdBoundMethod*>(callee);
        if (bm != nullptr && bm->bound_method != nullptr) {
            return bm->bound_method->fn;
        }
    }
    if (callee->Type() == OBJ_COMPILED_CLASS) {
        auto* klass = static_cast<AdCompiledClass*>(callee);
        if (klass == nullptr) {
            return nullptr;
        }
        auto it = klass->methods.find("constructor");
        if (it != klass->methods.end() && it->second != nullptr && it->second->fn != nullptr) {
            return it->second->fn;
        }
    }
    return nullptr;
}

void VM::execute_call_kw(int num_pos, int num_kw) {
    std::unordered_map<std::string, Ad_Object*> kw_map;
    for (int i = 0; i < num_kw; ++i) {
        Ad_Object* name_obj = pop();
        Ad_Object* val = pop();
        kw_map[vm_property_field_name(name_obj)] = val;
    }

    const int callee_index = sp - 1 - num_pos;
    if (callee_index < 0) {
        std::cerr << "[ VM Error ] OP_CALL_KW: stack underflow\n";
        return;
    }

    Ad_Object* callee = stack[callee_index];
    AdCompiledFunction* fn = vm_function_from_callee(callee);
    if (fn == nullptr) {
        std::cerr << "[ VM Error ] OP_CALL_KW: unsupported callee type\n";
        sp = callee_index;
        push(&NULLOBJECT);
        return;
    }

    int remaining_params = fn->num_parameters;
    for (const std::string& pname : fn->parameter_names) {
        if (kw_map.find(pname) != kw_map.end()) {
            --remaining_params;
        }
    }
    if (remaining_params > num_pos + static_cast<int>(fn->default_arg_values.size())) {
        sp = callee_index;
        auto* err = new Ad_Error_Object("some error message here");
        if (gc != nullptr) {
            gc->addObject(err);
        }
        push(err);
        return;
    }

    std::vector<Ad_Object*> resolved(static_cast<size_t>(fn->num_parameters), nullptr);
    for (int i = 0; i < num_pos && i < fn->num_parameters; ++i) {
        resolved[static_cast<size_t>(i)] = stack[callee_index + 1 + i];
    }

    const int num_defaults = static_cast<int>(fn->default_arg_values.size());
    const int threshold = num_defaults - fn->num_parameters + num_pos;
    for (int i = 0; i < num_defaults; ++i) {
        if (i >= threshold) {
            const int param_idx = fn->num_parameters - num_defaults + i;
            if (param_idx >= 0 && param_idx < fn->num_parameters &&
                resolved[static_cast<size_t>(param_idx)] == nullptr) {
                resolved[static_cast<size_t>(param_idx)] = fn->default_arg_values[static_cast<size_t>(i)];
            }
        }
    }

    for (const auto& entry : kw_map) {
        for (size_t i = 0; i < fn->parameter_names.size(); ++i) {
            if (fn->parameter_names[i] == entry.first) {
                resolved[i] = entry.second;
            }
        }
    }

    for (int i = 0; i < fn->num_parameters; ++i) {
        if (resolved[static_cast<size_t>(i)] == nullptr) {
            sp = callee_index;
            auto* err = new Ad_Error_Object("some error message here");
            if (gc != nullptr) {
                gc->addObject(err);
            }
            push(err);
            return;
        }
    }

    sp = callee_index;
    push(callee);
    for (Ad_Object* arg : resolved) {
        push(arg);
    }
    if (callee->Type() == OBJ_COMPILED_CLASS) {
        call_class(static_cast<AdCompiledClass*>(callee), fn->num_parameters);
        return;
    }

    execute_call(fn->num_parameters);
}

void VM::call_builtin(Ad_Builtin_Object* builtin, int num_args) {
    std::vector<Ad_Object*> args;
    args.reserve(static_cast<size_t>(num_args));
    for (int i = sp - num_args; i < sp; ++i) {
        args.push_back(stack[i]);
    }
    sp = sp - num_args - 1;
    Ad_Object* result = nullptr;
    if (builtin->builtin_function == locals_builtin) {
        result = build_locals_hash();
    } else {
        result = builtin->builtin_function(args, nullptr, gc);
    }
    if (result != nullptr) {
        push(result);
    } else {
        push(&NULLOBJECT);
    }
}

void VM::call_class(AdCompiledClass* cl, int num_args) {
    const int callee_index = sp - 1 - num_args;
    if (callee_index < 0) {
        std::cerr << "[ VM Error ] call_class: stack underflow\n";
        return;
    }

    std::vector<Ad_Object*> args;
    args.reserve(static_cast<size_t>(num_args));
    for (int i = 0; i < num_args; ++i) {
        args.push_back(stack[callee_index + 1 + i]);
    }
    sp = callee_index;

    auto* instance = new AdCompiledInstance();
    instance->klass = cl;
    instance->definition_num_args = num_args;
    if (gc != nullptr) {
        gc->addObject(instance);
    }

    for (AdCompiledFunction* field_initializer : cl->field_initializers) {
        auto* closure = new AdClosureObject();
        closure->fn = field_initializer;
        AdBoundMethod bound_initializer(instance, closure);
        const int frames_before = frames_index;
        const int sp_before = sp;
        call_bound_method(&bound_initializer, 0);
        run_until_frames_index(frames_before);
        while (sp > sp_before) {
            pop();
        }
    }

    auto ctor_it = cl->methods.find("constructor");
    if (ctor_it != cl->methods.end()) {
        AdBoundMethod bound_constructor(instance, ctor_it->second);
        const int frames_before = frames_index;
        const int sp_before = sp;
        for (Ad_Object* arg : args) {
            push(arg);
        }
        call_bound_method(&bound_constructor, num_args);
        run_until_frames_index(frames_before);
        while (sp > sp_before) {
            pop();
        }
    }

    push(instance);
}

void VM::call_bound_method(AdBoundMethod* bm, int num_args) {
    if (bm == nullptr || bm->bound_method == nullptr || bm->bound_method->fn == nullptr) {
        std::cerr << "[ VM Error ] call_bound_method: invalid bound method\n";
        return;
    }
    const int callee_index = sp - 1 - num_args;
    apply_default_arguments(bm->bound_method->fn, num_args);
    if (num_args != bm->bound_method->fn->num_parameters) {
        sp = callee_index;
        std::cerr << "ERROR: wrong number of arguments expecting: " << bm->bound_method->fn->num_parameters
                  << " got: " << num_args << std::endl;
        push(&NULLOBJECT);
        return;
    }
    Frame frame(bm->bound_method, -1, sp - num_args, bm->owner);
    push_frame(frame);
    sp = frame.base_pointer + bm->bound_method->fn->num_locals;
}

void VM::call_runtime_bound_method(AdRuntimeBoundMethod* bm, int num_args) {
    if (bm == nullptr || bm->receiver == nullptr) {
        std::cerr << "[ VM Error ] call_runtime_bound_method: invalid bound method\n";
        return;
    }

    std::vector<Ad_Object*> args;
    args.reserve(static_cast<size_t>(num_args));
    for (int i = sp - num_args; i < sp; ++i) {
        args.push_back(stack[i]);
    }
    sp = sp - num_args - 1;

    Ad_Object* result = nullptr;
    if (bm->receiver->Type() == OBJ_FILE) {
        auto* owner = static_cast<Ad_File_Object*>(bm->receiver);
        if (bm->method_name == "read") {
            if (owner->_operator == "r") {
                std::string data = read_file_content(owner->filename);
                result = new Ad_String_Object(data);
                if (gc != nullptr) {
                    gc->addObject(result);
                }
            }
        } else if (bm->method_name == "write") {
            if (owner->_operator.find('w') != std::string::npos) {
                if (!args.empty() && args[0] != nullptr) {
                    write_file_content(owner->filename, args[0]->repr());
                }
                result = &NULLOBJECT;
            } else if (owner->_operator.find('a') != std::string::npos) {
                if (!args.empty() && args[0] != nullptr) {
                    append_file_content(owner->filename, args[0]->repr());
                }
                result = &NULLOBJECT;
            }
        }
    } else if (bm->receiver->Type() == OBJ_SOCKET) {
        const std::string& method = bm->method_name;
        if (method == "create_server") {
            create_server(bm->receiver);
            result = &NULLOBJECT;
        } else if (method == "create_client") {
            create_client2(bm->receiver);
            result = &NULLOBJECT;
        } else if (method == "accept") {
            result = accept_socket(bm->receiver);
            if (gc != nullptr && result != nullptr) {
                gc->addObject(result);
            }
        } else if (method == "send") {
            if (!args.empty()) {
                send_socket(bm->receiver, args.at(0));
            }
            result = &NULLOBJECT;
        } else if (method == "read") {
            result = read_socket(bm->receiver);
            if (gc != nullptr && result != nullptr) {
                gc->addObject(result);
            }
        } else if (method == "readHTTP") {
            result = readHTTP(bm->receiver);
            if (gc != nullptr && result != nullptr) {
                gc->addObject(result);
            }
        } else if (method == "sendAndReadBackHTTP") {
            if (!args.empty()) {
                result = sendAndReadBackHTTP(bm->receiver, args.at(0));
                if (gc != nullptr && result != nullptr) {
                    gc->addObject(result);
                }
            }
        } else if (method == "sendAndReadBackHTTPS") {
            if (!args.empty()) {
                result = sendAndReadBackHTTPS(bm->receiver, args.at(0));
                if (gc != nullptr && result != nullptr) {
                    gc->addObject(result);
                }
            }
        } else if (method == "close") {
            close_socket(bm->receiver);
            result = &NULLOBJECT;
        } else {
            std::cout << "[ Ad ][ sock ] unknown method called on sock object\n";
            result = &NULLOBJECT;
        }
    } else if (bm->receiver->Type() == OBJ_THREAD) {
        static Environment thread_env;
        const std::string& method = bm->method_name;
        if (method == "callback" || method == "execute") {
            thread_callback(bm->receiver, args);
            result = &NULLOBJECT;
        } else if (method == "runAsync" || method == "start") {
            thread_async_run(bm->receiver, gc, thread_env);
            result = &NULLOBJECT;
        } else if (method == "runBlocking" || method == "join") {
            thread_blocking_run(bm->receiver, gc, thread_env);
            result = &NULLOBJECT;
        } else if (method == "await") {
            thread_await(bm->receiver, gc, thread_env);
            auto* thread_object = static_cast<Ad_Thread_Object*>(bm->receiver);
            result = thread_object->result != nullptr ? thread_object->result : &NULLOBJECT;
        } else {
            result = &NULLOBJECT;
        }
    }

    if (result != nullptr) {
        push(result);
    } else {
        push(&NULLOBJECT);
    }
}

Ad_Object *VM::last_popped_stack_element() {
    if (sp <= 0) {
        return nullptr;
    }
    return stack[sp - 1];
}

Frame* VM::current_frame() {
    if (frames_index == 0) {
        return nullptr;
    }
    return &frames[frames_index - 1];
}

void VM::push_frame(const Frame& f) {
    if (frames_index >= static_cast<int>(frames.size())) {
        frames.push_back(f);
    } else {
        frames[frames_index] = f;
    }
    frames_index += 1;
}

Frame VM::pop_frame() {
    frames_index -= 1;
    return frames[frames_index];
}

void VM::push(Ad_Object* obj) {
    if (sp >= stackSize) {
        std::cerr << "[ VM Error ] Stack overflow!" << std::endl;
        return;
    }
    stack[sp] = obj;
    sp++;
}

Ad_Object* VM::pop() {
    if (sp - 1 < 0) {
        std::cout << "[ VM Error ] Stack underflow! stack error: index out of bounds" << std::endl;
        return nullptr;
    }
    Ad_Object* result = stack[sp - 1];
    sp--;
    return result;
}

void VM::execute_binary_operation(OpCodeType opcode) {
    if (sp < 2) {
        std::cerr << "Not enough elements on stack for binary operation" << std::endl;
        return;
    }
    
    Ad_Object* right = pop();
    Ad_Object* left = pop();
        
    // Perform the operation based on the opcode
    Ad_Object* result = nullptr;
    
    bool left_is_int = left->Type() == OBJ_INT;
    bool right_is_int = right->Type() == OBJ_INT;
    bool left_is_float = left->Type() == OBJ_FLOAT;
    bool right_is_float = right->Type() == OBJ_FLOAT;

    bool left_is_string = left->Type() == OBJ_STRING;
    bool right_is_string = right->Type() == OBJ_STRING;

    if ((left_is_int || left_is_float) && (right_is_int || right_is_float)) {
        bool use_float = left_is_float || right_is_float;
        float left_val = left_is_float ? static_cast<Ad_Float_Object*>(left)->value
                                       : static_cast<float>(static_cast<Ad_Integer_Object*>(left)->value);
        float right_val = right_is_float ? static_cast<Ad_Float_Object*>(right)->value
                                         : static_cast<float>(static_cast<Ad_Integer_Object*>(right)->value);

        switch (opcode) {
            case OP_ADD:
                if (use_float) {
                    result = new Ad_Float_Object(left_val + right_val);
                } else {
                    result = new Ad_Integer_Object(static_cast<int>(left_val + right_val));
                }
                break;
            case OP_SUB:
                if (use_float) {
                    result = new Ad_Float_Object(left_val - right_val);
                } else {
                    result = new Ad_Integer_Object(static_cast<int>(left_val - right_val));
                }
                break;
            case OP_MULTIPLY:
                if (use_float) {
                    result = new Ad_Float_Object(left_val * right_val);
                } else {
                    result = new Ad_Integer_Object(static_cast<int>(left_val * right_val));
                }
                break;
            case OP_DIVIDE:
                if (right_val == 0.0f) {
                    std::cerr << "[ VM Error ] Division by zero!" << std::endl;
                    return;
                }
                if (use_float) {
                    result = new Ad_Float_Object(left_val / right_val);
                } else {
                    result = new Ad_Integer_Object(static_cast<int>(left_val / right_val));
                }
                break;
            default:
                std::cerr << "[ VM Error ] Unknown binary operation opcode: " << opcode << std::endl;
                return;
        }
    } else if (opcode == OP_ADD) {
        if (left_is_string || right_is_string) {
            const std::string left_str = left_is_string
                                             ? static_cast<Ad_String_Object*>(left)->value
                                             : (left != nullptr ? left->repr() : "null");
            const std::string right_str = right_is_string
                                              ? static_cast<Ad_String_Object*>(right)->value
                                              : (right != nullptr ? right->repr() : "null");
            result = new Ad_String_Object(left_str + right_str);
        } else {
            std::cerr << "[ VM Error ] Unsupported types for binary operation" << std::endl;
            return;
        }
    } else {
        std::cerr << "[ VM Error ] Unsupported types for binary operation" << std::endl;
        return;
    }
    
    if (result != nullptr) {
        push(result);
    }
}

void VM::execute_comparison(OpCodeType opcode) {
    if (sp < 2) {
        std::cerr << "[ VM Error ] Not enough elements on stack for comparison" << std::endl;
        return;
    }

    Ad_Object* right = pop();
    Ad_Object* left = pop();

    bool left_is_int = left->Type() == OBJ_INT;
    bool right_is_int = right->Type() == OBJ_INT;
    bool left_is_float = left->Type() == OBJ_FLOAT;
    bool right_is_float = right->Type() == OBJ_FLOAT;

    bool left_is_string = left->Type() == OBJ_STRING;
    bool right_is_string = right->Type() == OBJ_STRING;

    if ((left_is_int || left_is_float) && (right_is_int || right_is_float)) {
        float left_val = left_is_float ? static_cast<Ad_Float_Object*>(left)->value
                                       : static_cast<float>(static_cast<Ad_Integer_Object*>(left)->value);
        float right_val = right_is_float ? static_cast<Ad_Float_Object*>(right)->value
                                         : static_cast<float>(static_cast<Ad_Integer_Object*>(right)->value);
        bool comparison_result = false;
        switch (opcode) {
            case OP_EQUAL:
                comparison_result = left_val == right_val;
                break;
            case OP_NOTEQUAL:
                comparison_result = left_val != right_val;
                break;
            case OP_GREATERTHAN:
                comparison_result = left_val > right_val;
                break;
            case OP_GREATERTHAN_EQUAL:
                comparison_result = left_val >= right_val;
                break;
            default:
                std::cerr << "[ VM Error ] Unknown comparison opcode: " << opcode << std::endl;
                return;
        }
        push(native_bool_to_boolean_object(comparison_result));
        return;
    }

    if (left_is_string && right_is_string) {
        const std::string& left_val = static_cast<Ad_String_Object*>(left)->value;
        const std::string& right_val = static_cast<Ad_String_Object*>(right)->value;
        bool comparison_result = false;
        switch (opcode) {
            case OP_EQUAL:
                comparison_result = left_val == right_val;
                break;
            case OP_NOTEQUAL:
                comparison_result = left_val != right_val;
                break;
            default:
                std::cerr << "[ VM Error ] Unsupported types for comparison operation" << std::endl;
                return;
        }
        push(native_bool_to_boolean_object(comparison_result));
        return;
    }

    if (left->Type() == OBJ_NULL || right->Type() == OBJ_NULL) {
        bool both_null = is_nullish(left) && is_nullish(right);
        bool comparison_result = false;
        switch (opcode) {
            case OP_EQUAL:
                comparison_result = both_null;
                break;
            case OP_NOTEQUAL:
                comparison_result = !both_null;
                break;
            default:
                std::cerr << "[ VM Error ] Unsupported types for comparison operation" << std::endl;
                return;
        }
        push(native_bool_to_boolean_object(comparison_result));
        return;
    }

    if (left->Type() == OBJ_HASH && right->Type() == OBJ_HASH) {
        bool equal = vm_objects_equal(left, right);
        bool comparison_result = false;
        switch (opcode) {
            case OP_EQUAL:
                comparison_result = equal;
                break;
            case OP_NOTEQUAL:
                comparison_result = !equal;
                break;
            default:
                std::cerr << "[ VM Error ] Unsupported types for comparison operation" << std::endl;
                return;
        }
        push(native_bool_to_boolean_object(comparison_result));
        return;
    }

    if (left->Type() == OBJ_LIST && right->Type() == OBJ_LIST) {
        bool equal = vm_objects_equal(left, right);
        bool comparison_result = false;
        switch (opcode) {
            case OP_EQUAL:
                comparison_result = equal;
                break;
            case OP_NOTEQUAL:
                comparison_result = !equal;
                break;
            default:
                std::cerr << "[ VM Error ] Unsupported types for comparison operation" << std::endl;
                return;
        }
        push(native_bool_to_boolean_object(comparison_result));
        return;
    }

    bool result = false;
    switch (opcode) {
        case OP_EQUAL:
            result = left == right;
            break;
        case OP_NOTEQUAL:
            result = left != right;
            break;
        default:
            std::cerr << "[ VM Error ] Unsupported types for comparison operation" << std::endl;
            return;
    }
    push(native_bool_to_boolean_object(result));
}

void VM::execute_bang_operator() {
    Ad_Object* operand = pop();
    push(native_bool_to_boolean_object(!is_truthy(operand)));
}

void VM::execute_minus_operator() {
    Ad_Object* operand = pop();
    if (operand == nullptr) {
        std::cerr << "[ VM Error ] Null operand for unary minus" << std::endl;
        return;
    }
    if (operand->Type() == OBJ_INT) {
        push(new Ad_Integer_Object(-static_cast<Ad_Integer_Object*>(operand)->value));
        return;
    }
    if (operand->Type() == OBJ_FLOAT) {
        push(new Ad_Float_Object(-static_cast<Ad_Float_Object*>(operand)->value));
        return;
    }
    std::cerr << "[ VM Error ] Unsupported type for unary minus" << std::endl;
}

Ad_Object* VM::native_bool_to_boolean_object(bool value) {
    if (value) {
        return &TRUE;
    }
    return &FALSE;
}

bool VM::is_truthy(Ad_Object* obj) {
    if (obj == nullptr) return false;
    if (obj == &NULLOBJECT) return false;
    if (obj == &TRUE) return true;
    if (obj == &FALSE) return false;
    return true;
}

Ad_Object* VM::build_array(int start_index, int end_index) {
    std::vector<Ad_Object*> elements;
    for (int i = start_index; i < end_index; i++) {
        elements.push_back(stack[i]);
    }
    return new Ad_List_Object(elements);
}

Ad_Object* VM::build_hash(int start_index, int end_index) {
    std::unordered_map<std::string, HashPair> hashed_pairs;
    std::hash<std::string> hash_string;
    for (int i = start_index; i < end_index; i += 2) {
        Ad_Object* key = stack[i];
        Ad_Object* value = stack[i + 1];
        HashPair pair(key, value);
        std::string hash_key = std::to_string(hash_string(key->Hash()));
        hashed_pairs[hash_key] = pair;
    }
    return new Ad_Hash_Object(hashed_pairs);
}

void VM::execute_index_expression(Ad_Object* left, Ad_Object* index) {
    if (left->Type() == OBJ_LIST) {
        execute_array_index(left, index);
        return;
    }
    if (left->Type() == OBJ_HASH) {
        execute_hash_index(left, index);
        return;
    }
    if (left->Type() == OBJ_STRING) {
        execute_string_index(left, index);
        return;
    }
    push(&NULLOBJECT);
}

void VM::execute_set_index_expression(Ad_Object* left, Ad_Object* index, Ad_Object* value) {
    if (left == nullptr || index == nullptr) {
        return;
    }
    if (left->Type() == OBJ_LIST && index->Type() == OBJ_INT) {
        int i = static_cast<Ad_Integer_Object*>(index)->value;
        auto* list_obj = static_cast<Ad_List_Object*>(left);
        if (i >= 0 && i < static_cast<int>(list_obj->elements.size())) {
            list_obj->elements[static_cast<size_t>(i)] = value;
        }
        return;
    }
    if (left->Type() == OBJ_HASH) {
        std::hash<std::string> hash_string;
        auto* hash_obj = static_cast<Ad_Hash_Object*>(left);
        HashPair hash_pair(index, value);
        std::string hash_key = std::to_string(hash_string(index->Hash()));
        auto it = hash_obj->pairs.find(hash_key);
        if (it == hash_obj->pairs.end()) {
            hash_obj->pairs.insert(std::make_pair(hash_key, hash_pair));
        } else {
            it->second = hash_pair;
        }
    }
}

void VM::execute_array_index(Ad_Object* left, Ad_Object* index) {
    int i = ((Ad_Integer_Object*)index)->value;
    int max = static_cast<int>(((Ad_List_Object*)left)->elements.size());
    if (i < 0 || i >= max) {
        push(&NULLOBJECT);
    } else {
        push(((Ad_List_Object*)left)->elements[i]);
    }
}

void VM::execute_string_index(Ad_Object* left, Ad_Object* index) {
    if (index == nullptr || index->Type() != OBJ_INT) {
        push(&NULLOBJECT);
        return;
    }
    auto* str_obj = static_cast<Ad_String_Object*>(left);
    const int idx = static_cast<Ad_Integer_Object*>(index)->value;
    if (idx < 0 || idx >= static_cast<int>(str_obj->value.size())) {
        push(&NULLOBJECT);
        return;
    }
    push(new Ad_String_Object(str_obj->value.substr(static_cast<size_t>(idx), 1)));
}

void VM::execute_slice_expression(Ad_Object* left, Ad_Object* start, Ad_Object* end, Ad_Object* step) {
    if (left == nullptr) {
        push(&NULLOBJECT);
        return;
    }

    const bool start_is_int = !is_nullish(start) && start->Type() == OBJ_INT;
    const bool end_is_int = !is_nullish(end) && end->Type() == OBJ_INT;
    const bool step_is_int = !is_nullish(step) && step->Type() == OBJ_INT;

    if (left->Type() == OBJ_LIST) {
        if (start_is_int && end_is_int && (is_nullish(step) || step_is_int)) {
            int i3 = step_is_int ? int_value(step) : 1;
            push(vm_new_sub_list(left, int_value(start), int_value(end), i3, gc));
            return;
        }
        if (is_nullish(start) && end_is_int && (is_nullish(step) || step_is_int)) {
            int i3 = step_is_int ? int_value(step) : 1;
            int i1 = i3 > 0 ? 0 : INT_MAX;
            push(vm_new_sub_list(left, i1, int_value(end), i3, gc));
            return;
        }
        if (start_is_int && is_nullish(end) && (is_nullish(step) || step_is_int)) {
            int i3 = step_is_int ? int_value(step) : 1;
            int i2 = i3 > 0 ? static_cast<int>(static_cast<Ad_List_Object*>(left)->elements.size()) : -INT_MAX;
            push(vm_new_sub_list(left, int_value(start), i2, i3, gc));
            return;
        }
        if (is_nullish(start) && is_nullish(end) && (is_nullish(step) || step_is_int)) {
            int i3 = step_is_int ? int_value(step) : 1;
            if (i3 > 0) {
                push(vm_new_sub_list(left, 0, static_cast<int>(static_cast<Ad_List_Object*>(left)->elements.size()), i3, gc));
            } else {
                push(vm_new_sub_list(left, static_cast<int>(static_cast<Ad_List_Object*>(left)->elements.size()) - 1, -INT_MAX, i3, gc));
            }
            return;
        }
        if (is_nullish(start) && is_nullish(end) && is_nullish(step)) {
            int max = static_cast<int>(static_cast<Ad_List_Object*>(left)->elements.size());
            push(vm_new_sub_list(left, 0, max, 1, gc));
            return;
        }
    }

    if (left->Type() == OBJ_STRING) {
        if (start_is_int && end_is_int && (is_nullish(step) || step_is_int)) {
            int max = static_cast<int>(static_cast<Ad_String_Object*>(left)->value.size());
            int idx = int_value(start);
            int idx_end = int_value(end);
            int idx_step = step_is_int ? int_value(step) : 1;

            if (idx < -max) idx = -max;
            if (idx < 0) idx += max;
            if (idx >= max) idx = max;
            if (idx_end < -max) idx_end = -max;
            if (idx_end < 0) idx_end += max;
            if (idx_end >= max) idx_end = max;
            if (idx < idx_end && idx_step < 0) {
                push(new Ad_String_Object(""));
                return;
            }
            if (idx > idx_end && idx_step > 0) {
                push(new Ad_String_Object(""));
                return;
            }
            push(vm_new_sub_string(left, int_value(start), int_value(end), idx_step, gc));
            return;
        }
        if (is_nullish(start) && end_is_int && (is_nullish(step) || step_is_int)) {
            int inc = step_is_int ? int_value(step) : 1;
            int end_val = int_value(end);
            auto* target = static_cast<Ad_String_Object*>(left);
            if (end_val < 0) {
                end_val += static_cast<int>(target->value.size());
            }
            push(vm_new_sub_string(target, 0, end_val, inc, gc));
            return;
        }
        if (start_is_int && is_nullish(end) && (is_nullish(step) || step_is_int)) {
            auto* target = static_cast<Ad_String_Object*>(left);
            int inc = step_is_int ? int_value(step) : 1;
            push(vm_new_sub_string(target, int_value(start), static_cast<int>(target->value.size()), inc, gc));
            return;
        }
        if (is_nullish(start) && is_nullish(end) && (is_nullish(step) || step_is_int)) {
            auto* target = static_cast<Ad_String_Object*>(left);
            int inc = step_is_int ? int_value(step) : 1;
            int end_val = inc >= 0 ? static_cast<int>(target->value.size()) : static_cast<int>(target->value.size()) - 1;
            push(vm_new_sub_string(target, 0, end_val, inc, gc));
            return;
        }
        if (is_nullish(start) && is_nullish(end) && is_nullish(step)) {
            push(new Ad_String_Object(static_cast<Ad_String_Object*>(left)->value));
            return;
        }
    }

    push(&NULLOBJECT);
}

void VM::execute_hash_index(Ad_Object* left, Ad_Object* index) {
    std::hash<std::string> hash_string;
    std::string hash_key = std::to_string(hash_string(index->Hash()));
    auto it = ((Ad_Hash_Object*)left)->pairs.find(hash_key);
    if (it != ((Ad_Hash_Object*)left)->pairs.end()) {
        push(it->second.value);
    } else {
        push(&NULLOBJECT);
    }
}

AdCompiledInstance* VM::current_bound_instance() {
    for (int i = frames_index - 1; i >= 0; --i) {
        if (frames[i].bound_instance != nullptr) {
            return frames[i].bound_instance;
        }
    }
    return nullptr;
}

Ad_Object* VM::build_locals_hash() {
    std::unordered_map<std::string, HashPair> pairs;
    std::hash<std::string> hash_string;

    Frame* frame = current_frame();
    if (frame != nullptr && frame->cl != nullptr && frame->cl->fn != nullptr && frames_index > 1) {
        for (const auto& entry : frame->cl->fn->local_names) {
            const int slot = frame->base_pointer + entry.second;
            if (slot < 0 || slot >= stackSize) {
                continue;
            }
            Ad_Object* value = stack[slot];
            if (value == nullptr) {
                continue;
            }
            auto* key = new Ad_String_Object(entry.first);
            if (gc != nullptr) {
                gc->addObject(key);
            }
            HashPair hash_pair(key, value);
            pairs.insert(std::make_pair(std::to_string(hash_string(key->Hash())), hash_pair));
        }
        auto* hash_object = new Ad_Hash_Object(pairs);
        if (gc != nullptr) {
            gc->addObject(hash_object);
        }
        return hash_object;
    }

    for (size_t i = 0; i < global_names.size(); ++i) {
        const std::string& name = global_names[i];
        if (name.empty()) {
            continue;
        }
        if (bootstrap_global_names.count(name) > 0) {
            continue;
        }
        if (i >= globals.size() || globals[i] == nullptr) {
            continue;
        }
        auto* key = new Ad_String_Object(name);
        if (gc != nullptr) {
            gc->addObject(key);
        }
        HashPair hash_pair(key, globals[i]);
        pairs.insert(std::make_pair(std::to_string(hash_string(key->Hash())), hash_pair));
    }

    auto* hash_object = new Ad_Hash_Object(pairs);
    if (gc != nullptr) {
        gc->addObject(hash_object);
    }
    return hash_object;
}

void VM::ensure_instance_field_capacity(AdCompiledInstance* inst, int index) {
    if (inst == nullptr || index < 0) {
        return;
    }
    const size_t needed = static_cast<size_t>(index) + 1;
    if (inst->fields.size() < needed) {
        inst->fields.resize(needed, nullptr);
    }
}

void VM::register_instance_field_name(AdCompiledInstance* inst, const std::string& name, int index) {
    if (inst == nullptr || inst->klass == nullptr) {
        return;
    }
    inst->klass->field_name_to_index[name] = index;
}

int VM::lookup_instance_field_index(AdCompiledInstance* inst, const std::string& name) {
    if (inst == nullptr || inst->klass == nullptr) {
        return -1;
    }
    auto it = inst->klass->field_name_to_index.find(name);
    if (it == inst->klass->field_name_to_index.end()) {
        return -1;
    }
    return it->second;
}

std::string vm_property_field_name(Ad_Object* field_name_obj) {
    if (field_name_obj == nullptr) {
        return "";
    }
    if (field_name_obj->Type() == OBJ_STRING) {
        return static_cast<Ad_String_Object*>(field_name_obj)->value;
    }
    return field_name_obj->Inspect();
}

void VM::push_bound_instance_member(AdCompiledInstance* inst, const std::string& name) {
    if (inst == nullptr) {
        push(&NULLOBJECT);
        return;
    }
    const int index = lookup_instance_field_index(inst, name);
    if (index >= 0 && index < static_cast<int>(inst->fields.size())) {
        Ad_Object* value = inst->fields[static_cast<size_t>(index)];
        if (value != nullptr) {
            push(value);
            return;
        }
    }
    if (inst->klass != nullptr) {
        auto it = inst->klass->methods.find(name);
        if (it != inst->klass->methods.end() && it->second != nullptr) {
            auto* bound = new AdBoundMethod(inst, it->second);
            if (gc != nullptr) {
                gc->addObject(bound);
            }
            push(bound);
            return;
        }
    }
    push(&NULLOBJECT);
}

void VM::execute_get_property_sym(int sym_index) {
    if (sp <= 0) {
        std::cerr << "[ VM Error ] OP_GET_PROPERTY_SYM: stack underflow\n";
        return;
    }
    Ad_Object* field_name_obj = pop();

    AdCompiledInstance* inst = current_bound_instance();
    if (inst == nullptr) {
        std::cerr << "[ VM Error ] OP_GET_PROPERTY_SYM: no bound instance\n";
        push(&NULLOBJECT);
        return;
    }
    int index = sym_index;
    if (sym_index == 65535) {
        const std::string field_name = vm_property_field_name(field_name_obj);
        push_bound_instance_member(inst, field_name);
        return;
    }
    if (index < 0 || index >= static_cast<int>(inst->fields.size())) {
        push(&NULLOBJECT);
        return;
    }
    Ad_Object* value = inst->fields[static_cast<size_t>(index)];
    if (value != nullptr) {
        push(value);
    } else {
        push(&NULLOBJECT);
    }
}

void VM::execute_patch_property_sym(int sym_index) {
    if (sp < 2) {
        std::cerr << "[ VM Error ] OP_PATCH_PROPERTY_SYM: stack underflow\n";
        return;
    }
    Ad_Object* field_name_obj = pop();
    Ad_Object* value = pop();

    AdCompiledInstance* inst = current_bound_instance();
    if (inst == nullptr) {
        std::cerr << "[ VM Error ] OP_PATCH_PROPERTY_SYM: no bound instance\n";
        return;
    }
    const std::string field_name = vm_property_field_name(field_name_obj);
    int index = sym_index;
    if (sym_index == 65535) {
        index = lookup_instance_field_index(inst, field_name);
        if (index < 0) {
            index = static_cast<int>(inst->fields.size());
            register_instance_field_name(inst, field_name, index);
        }
    }
    ensure_instance_field_capacity(inst, index);
    inst->fields[static_cast<size_t>(index)] = value;
    if (sym_index != 65535) {
        register_instance_field_name(inst, field_name, index);
    }
}

void VM::execute_get_property() {
    if (sp < 2) {
        std::cerr << "[ VM Error ] OP_GET_PROPERTY: stack underflow\n";
        return;
    }
    Ad_Object* field_name_obj = pop();
    Ad_Object* owner = pop();
    if (owner == nullptr || owner->Type() != OBJ_COMPILED_INSTANCE) {
        push(&NULLOBJECT);
        return;
    }
    auto* inst = static_cast<AdCompiledInstance*>(owner);
    const std::string field_name = vm_property_field_name(field_name_obj);
    const int index = lookup_instance_field_index(inst, field_name);
    if (index < 0 || index >= static_cast<int>(inst->fields.size())) {
        push(&NULLOBJECT);
        return;
    }
    Ad_Object* value = inst->fields[static_cast<size_t>(index)];
    if (value != nullptr) {
        push(value);
    } else {
        push(&NULLOBJECT);
    }
}

void VM::execute_set_property() {
    if (sp < 3) {
        std::cerr << "[ VM Error ] OP_SET_PROPERTY: stack underflow\n";
        return;
    }
    Ad_Object* field_name_obj = pop();
    Ad_Object* value = pop();
    Ad_Object* owner = pop();
    if (owner == nullptr || owner->Type() != OBJ_COMPILED_INSTANCE) {
        std::cerr << "[ VM Error ] OP_SET_PROPERTY: owner is not a compiled instance\n";
        return;
    }
    auto* inst = static_cast<AdCompiledInstance*>(owner);
    const std::string field_name = vm_property_field_name(field_name_obj);
    int index = lookup_instance_field_index(inst, field_name);
    if (index < 0) {
        index = static_cast<int>(inst->fields.size());
        register_instance_field_name(inst, field_name, index);
    }
    ensure_instance_field_capacity(inst, index);
    inst->fields[static_cast<size_t>(index)] = value;
}

void VM::execute_get_method() {
    if (sp < 2) {
        std::cerr << "[ VM Error ] OP_GET_METHOD: stack underflow\n";
        return;
    }
    Ad_Object* name_obj = pop();
    Ad_Object* owner = pop();
    if (owner == nullptr) {
        push(&NULLOBJECT);
        return;
    }
    const std::string method_name = vm_property_field_name(name_obj);
    if (owner->Type() == OBJ_COMPILED_INSTANCE) {
        auto* inst = static_cast<AdCompiledInstance*>(owner);
        if (inst->klass == nullptr) {
            push(&NULLOBJECT);
            return;
        }
        auto it = inst->klass->methods.find(method_name);
        if (it != inst->klass->methods.end() && it->second != nullptr) {
            auto* bound = new AdBoundMethod(inst, it->second);
            if (gc != nullptr) {
                gc->addObject(bound);
            }
            push(bound);
            return;
        }
        const int field_index = lookup_instance_field_index(inst, method_name);
        if (field_index >= 0 && field_index < static_cast<int>(inst->fields.size())) {
            Ad_Object* field_val = inst->fields[static_cast<size_t>(field_index)];
            if (field_val != nullptr && field_val->Type() == OBJ_CLOSURE) {
                auto* bound = new AdBoundMethod(inst, static_cast<AdClosureObject*>(field_val));
                if (gc != nullptr) {
                    gc->addObject(bound);
                }
                push(bound);
                return;
            }
        }
        push(&NULLOBJECT);
        return;
    }
    if (owner->Type() == OBJ_FILE) {
        auto* bound = new AdRuntimeBoundMethod(owner, method_name);
        if (gc != nullptr) {
            gc->addObject(bound);
        }
        push(bound);
        return;
    }
    if (owner->Type() == OBJ_SOCKET || owner->Type() == OBJ_THREAD) {
        auto* bound = new AdRuntimeBoundMethod(owner, method_name);
        if (gc != nullptr) {
            gc->addObject(bound);
        }
        push(bound);
        return;
    }
    push(&NULLOBJECT);
}

void VM::execute_get_super_method() {
    if (sp < 3) {
        std::cerr << "[ VM Error ] OP_GET_SUPER_METHOD: stack underflow\n";
        return;
    }
    Ad_Object* method_name_obj = pop();
    Ad_Object* parent_name_obj = pop();
    Ad_Object* owner = pop();
    if (owner == nullptr || owner->Type() != OBJ_COMPILED_INSTANCE) {
        push(&NULLOBJECT);
        return;
    }
    auto* inst = static_cast<AdCompiledInstance*>(owner);
    if (inst->klass == nullptr) {
        push(&NULLOBJECT);
        return;
    }
    const std::string parent_name = vm_property_field_name(parent_name_obj);
    const std::string method_name = vm_property_field_name(method_name_obj);
    auto parent_it = inst->klass->super_classes_by_name.find(parent_name);
    if (parent_it == inst->klass->super_classes_by_name.end() || parent_it->second == nullptr) {
        push(&NULLOBJECT);
        return;
    }
    auto method_it = parent_it->second->methods.find(method_name);
    if (method_it == parent_it->second->methods.end() || method_it->second == nullptr) {
        push(&NULLOBJECT);
        return;
    }
    auto* bound = new AdBoundMethod(inst, method_it->second);
    if (gc != nullptr) {
        gc->addObject(bound);
    }
    push(bound);
}

void VM::execute_get_this() {
    AdCompiledInstance* inst = current_bound_instance();
    if (inst != nullptr) {
        push(inst);
    } else {
        push(&NULLOBJECT);
    }
}

void VM::execute_set_method() {
    if (sp < 3) {
        std::cerr << "[ VM Error ] OP_SET_METHOD: stack underflow\n";
        return;
    }
    Ad_Object* name_obj = pop();
    Ad_Object* method_obj = pop();
    if (method_obj == nullptr || method_obj->Type() != OBJ_CLOSURE) {
        std::cerr << "[ VM Error ] OP_SET_METHOD: expected closure\n";
        return;
    }
    if (sp <= 0 || stack[sp - 1] == nullptr || stack[sp - 1]->Type() != OBJ_COMPILED_CLASS) {
        std::cerr << "[ VM Error ] OP_SET_METHOD: expected compiled class on stack\n";
        return;
    }
    auto* klass = static_cast<AdCompiledClass*>(stack[sp - 1]);
    klass->methods[vm_property_field_name(name_obj)] = static_cast<AdClosureObject*>(method_obj);
}

Ad_Object* VM::invoke_closure(AdClosureObject* closure, const std::vector<Ad_Object*>& args) {
    if (closure == nullptr || closure->fn == nullptr) {
        return &NULLOBJECT;
    }

    VM runner;
    runner.gc = gc;
    runner.constants = constants;
    runner.globals = globals;
    runner.global_names = global_names;
    runner.bootstrap_global_names = bootstrap_global_names;
    runner.warn_return_outside_function = false;
    runner.sp = 0;
    runner.frames_index = 0;
    runner.frames.clear();

    // Arrange stack like `execute_call` expects: [callee, arg0, arg1, ...]
    runner.push(closure);
    for (Ad_Object* arg : args) {
        runner.push(arg != nullptr ? arg : &NULLOBJECT);
    }

    int num_args = static_cast<int>(args.size());
    runner.apply_default_arguments(closure->fn, num_args);
    Frame frame(closure, -1, runner.sp - num_args, nullptr);
    runner.push_frame(frame);
    runner.sp = frame.base_pointer + closure->fn->num_locals;

    runner.run();
    if (runner.sp > 0 && runner.stack[runner.sp - 1] != nullptr) {
        return runner.stack[runner.sp - 1];
    }
    return &NULLOBJECT;
}
