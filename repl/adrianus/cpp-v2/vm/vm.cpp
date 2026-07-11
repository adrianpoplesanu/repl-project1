#include "vm.h"
#include "code.h"
#include "objects.h"
#include "utils.h"
#include "../builtins_registry.h"
#include "../evaluator.h"
#include <iostream>
#include <vector>
#include <functional>
#include <cstdint>

VM::VM() {
    sp = 0;
    gc = nullptr;
    frames.clear();
    frames_index = 0;
    constants.clear();
    globals.clear();
    has_loaded_bytecode = false;
}

void VM::load(Bytecode bytecode) {
    last_loaded_bytecode = bytecode;
    has_loaded_bytecode = true;

    // Store constants from bytecode
    constants = bytecode.constants;
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
    } else if (opcode == OP_RETURN_VALUE) {
        Ad_Object* return_value = pop();
        Frame ret_frame = pop_frame();
        sp = ret_frame.base_pointer - 1;
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
    if (num_args < cl->fn->num_parameters) {
        // Match `Evaluator::ApplyFunction` too-few-args path (same error message).
        sp = callee_index;
        auto* err = new Ad_Error_Object("some error message here");
        if (gc != nullptr) {
            gc->addObject(err);
        }
        push(err);
        return;
    }
    apply_default_arguments(cl->fn, num_args);
    Frame frame(cl, -1, sp - num_args, current_bound_instance());
    push_frame(frame);
    sp = frame.base_pointer + cl->fn->num_locals;
}

void VM::call_builtin(Ad_Builtin_Object* builtin, int num_args) {
    std::vector<Ad_Object*> args;
    args.reserve(static_cast<size_t>(num_args));
    for (int i = sp - num_args; i < sp; ++i) {
        args.push_back(stack[i]);
    }
    sp = sp - num_args - 1;
    Ad_Object* result = builtin->builtin_function(args, nullptr, gc);
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
    if (num_args != bm->bound_method->fn->num_parameters) {
        // Defaults may reduce the gap; apply before the final arity check.
    }
    apply_default_arguments(bm->bound_method->fn, num_args);
    if (num_args != bm->bound_method->fn->num_parameters) {
        std::cerr << "ERROR: wrong number of arguments expecting: " << bm->bound_method->fn->num_parameters
                  << " got: " << num_args << std::endl;
    }
    Frame frame(bm->bound_method, -1, sp - num_args, bm->owner);
    push_frame(frame);
    sp = frame.base_pointer + bm->bound_method->fn->num_locals;
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

static std::string vm_property_field_name(Ad_Object* field_name_obj) {
    if (field_name_obj == nullptr) {
        return "";
    }
    if (field_name_obj->Type() == OBJ_STRING) {
        return static_cast<Ad_String_Object*>(field_name_obj)->value;
    }
    return field_name_obj->Inspect();
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
    if (sym_index < 0 || sym_index >= static_cast<int>(inst->fields.size())) {
        push(&NULLOBJECT);
        return;
    }
    Ad_Object* value = inst->fields[static_cast<size_t>(sym_index)];
    if (value != nullptr) {
        push(value);
    } else {
        push(&NULLOBJECT);
    }
    (void)field_name_obj;
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
    ensure_instance_field_capacity(inst, sym_index);
    inst->fields[static_cast<size_t>(sym_index)] = value;
    register_instance_field_name(inst, vm_property_field_name(field_name_obj), sym_index);
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
    if (owner == nullptr || owner->Type() != OBJ_COMPILED_INSTANCE) {
        push(&NULLOBJECT);
        return;
    }
    auto* inst = static_cast<AdCompiledInstance*>(owner);
    if (inst->klass == nullptr) {
        push(&NULLOBJECT);
        return;
    }
    const std::string method_name = vm_property_field_name(name_obj);
    auto it = inst->klass->methods.find(method_name);
    if (it == inst->klass->methods.end() || it->second == nullptr) {
        push(&NULLOBJECT);
        return;
    }
    auto* bound = new AdBoundMethod(inst, it->second);
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
