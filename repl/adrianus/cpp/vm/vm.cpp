#include "vm.h"
#include "code.h"
#include "objects.h"
#include "utils.h"
#include "../evaluator.h"
#include <iostream>
#include <vector>
#include <functional>

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

void VM::run() {
    while (current_frame()->ip < static_cast<int>(current_frame()->instructions()->bytes.size()) - 1) {
        current_frame()->ip += 1;
        int ip = current_frame()->ip;
        Instructions* ins = current_frame()->instructions();
        
        if (ip < 0 || ip >= static_cast<int>(ins->bytes.size())) {
            break;
        }
        
        OpCodeType opcode = static_cast<OpCodeType>(ins->bytes[ip]);
        
        if (opcode == OP_CONSTANT) {
            int const_index = read_uint16(*ins, ip + 1);
            current_frame()->ip += 2;
            if (const_index >= 0 && const_index < static_cast<int>(constants.size())) {
                push(constants[const_index]);
            }
        } else if (opcode == OP_ADD || opcode == OP_SUB || opcode == OP_MULTIPLY || opcode == OP_DIVIDE) {
            execute_binary_operation(opcode);
        } else if (opcode == OP_POP) {
            pop();
        } else if (opcode == OP_TRUE) {
            Ad_Object* obj = native_bool_to_boolean_object(true);
            push(obj);
        } else if (opcode == OP_FALSE) {
            Ad_Object* obj = native_bool_to_boolean_object(false);
            push(obj);
        } else if (opcode == OP_POP) {
            pop();
        } else if (opcode == OP_JUMP) {
            int pos = read_uint16(*ins, ip + 1);
            current_frame()->ip = pos - 1;
        } else if (opcode == OP_JUMP_NOT_TRUTHY) {
            int pos = read_uint16(*ins, ip + 1);
            current_frame()->ip += 2;

            Ad_Object* condition = pop();
            if (!is_truthy(condition)) {
                current_frame()->ip = pos - 1;
            }
        } else if (opcode == OP_NULL) {
            push(&NULLOBJECT);
        } else if (opcode == OP_SET_GLOBAL) {
            int global_index = read_uint16(*ins, ip + 1);
            current_frame()->ip += 2;

            // Ensure globals vector is large enough
            if (global_index >= static_cast<int>(globals.size())) {
                globals.resize(global_index + 1, nullptr);
            }
            globals[global_index] = pop();
        } else if (opcode == OP_GET_GLOBAL) {
            int global_index = read_uint16(*ins, ip + 1);
            current_frame()->ip += 2;

            if (global_index >= 0 && global_index < static_cast<int>(globals.size())) {
                push(globals[global_index]);
            } else {
                std::cerr << "[ VM Error ] Global index out of bounds: " << global_index << std::endl;
            }
        } else if (opcode == OP_SET_LOCAL) {
            int local_index = read_uint8(*ins, ip + 1);
            current_frame()->ip += 1;

            Frame* frame = current_frame();
            if (frame == nullptr) {
                std::cerr << "[ VM Error ] OP_SET_LOCAL: no current frame" << std::endl;
                continue;
            }
            int slot = frame->base_pointer + local_index;
            if (slot < 0 || slot >= stackSize) {
                std::cerr << "[ VM Error ] OP_SET_LOCAL: local slot out of bounds: " << slot << std::endl;
                continue;
            }
            stack[slot] = pop();
        } else if (opcode == OP_GET_LOCAL) {
            int local_index = read_uint8(*ins, ip + 1);
            current_frame()->ip += 1;

            Frame* frame = current_frame();
            if (frame == nullptr) {
                std::cerr << "[ VM Error ] OP_GET_LOCAL: no current frame" << std::endl;
                continue;
            }
            int slot = frame->base_pointer + local_index;
            if (slot < 0 || slot >= stackSize) {
                std::cerr << "[ VM Error ] OP_GET_LOCAL: local slot out of bounds: " << slot << std::endl;
                continue;
            }
            push(stack[slot]);
        } else if (opcode == OP_ARRAY) {
            int numElements = read_uint16(*ins, ip + 1);
            current_frame()->ip += 2;

            Ad_Object* array_obj = build_array(sp - numElements, sp);
            sp = sp - numElements;
            push(array_obj);
        } else if (opcode == OP_HASH) {
            int numElements = read_uint16(*ins, ip + 1);
            current_frame()->ip += 2;

            Ad_Object* hash_obj = build_hash(sp - numElements, sp);
            sp = sp - numElements;
            push(hash_obj);
        } else if (opcode == OP_INDEX) {
            Ad_Object* index = pop();
            Ad_Object* left = pop();
            execute_index_expression(left, index);
        } else if (opcode == OP_CLOSURE) {
            int const_index = read_uint16(*ins, ip + 1);
            int num_free = read_uint8(*ins, ip + 3);
            current_frame()->ip += 3;

            if (const_index < 0 || const_index >= static_cast<int>(constants.size())) {
                std::cerr << "[ VM Error ] OP_CLOSURE constant index out of bounds: " << const_index << std::endl;
                continue;
            }
            if (constants[const_index] == nullptr || constants[const_index]->Type() != OBJ_COMPILED_FUNCTION) {
                std::cerr << "[ VM Error ] OP_CLOSURE expected compiled function constant at index: " << const_index << std::endl;
                continue;
            }

            // Free variables are not wired in this VM object model yet.
            (void)num_free;

            auto* closure = new AdClosureObject();
            closure->fn = static_cast<AdCompiledFunction*>(constants[const_index]);
            push(closure);
        } else if (opcode == OP_CALL) {
            int num_args = read_uint8(*ins, ip + 1);
            current_frame()->ip += 1;
            execute_call(num_args);
        } else if (opcode == OP_RETURN_VALUE) {
            Ad_Object* return_value = pop();
            Frame frame = pop_frame();
            sp = frame.base_pointer - 1;
            push(return_value);
            if (frames_index == 0) {
                break;
            }
        } else if (opcode == OP_RETURN) {
            Frame frame = pop_frame();
            sp = frame.base_pointer - 1;
            if (sp < 0) {
                sp = 0;
            }
            push(&NULLOBJECT);
            if (frames_index == 0) {
                break;
            }
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
    }
}

void VM::call_closure(AdClosureObject* cl, int num_args) {
    if (cl == nullptr || cl->fn == nullptr) {
        std::cerr << "[ VM Error ] call_closure: null closure or function\n";
        return;
    }
    if (num_args != cl->fn->num_parameters) {
        std::cerr << "ERROR: wrong number of arguments expecting: " << cl->fn->num_parameters
                  << " got: " << num_args << std::endl;
    }
    Frame frame(cl, -1, sp - num_args, nullptr);
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
    auto* instance = new AdCompiledInstance();
    instance->klass = cl;
    instance->definition_num_args = num_args;
    bool need_to_remove = false;
    for (AdCompiledFunction* field_initializer : cl->field_initializers) {
        auto* closure = new AdClosureObject();
        closure->fn = field_initializer;
        push(closure);
        need_to_remove = true;
        AdBoundMethod bound_initializer(instance, closure);
        call_bound_method(&bound_initializer, 0);
        if (sp > 0 && stack[sp - 1] != nullptr) {
            pop();
        }
    }
    auto ctor_it = cl->methods.find("constructor");
    if (ctor_it != cl->methods.end()) {
        AdClosureObject* constructor = ctor_it->second;
        if (!need_to_remove) {
            pop();
        }
        AdBoundMethod bound_constructor(instance, constructor);
        bound_constructor.owner = instance;
        current_frame()->bound_instance = instance;
        call_bound_method(&bound_constructor, num_args);
    }
    push(instance);
}

void VM::call_bound_method(AdBoundMethod* bm, int num_args) {
    if (bm == nullptr || bm->bound_method == nullptr || bm->bound_method->fn == nullptr) {
        std::cerr << "[ VM Error ] call_bound_method: invalid bound method\n";
        return;
    }
    if (num_args != bm->bound_method->fn->num_parameters) {
        std::cerr << "ERROR: wrong number of arguments expecting: " << bm->bound_method->fn->num_parameters
                  << " got: " << num_args << std::endl;
    }
    int old_sp = sp;
    std::vector<Ad_Object*> popped;
    popped.reserve(static_cast<size_t>(num_args));
    for (int total = num_args; total > 0; --total) {
        popped.push_back(pop());
    }
    push(bm->owner);
    while (!popped.empty()) {
        push(popped.back());
        popped.pop_back();
    }
    Frame frame(bm->bound_method, -1, old_sp - num_args, bm->owner);
    push_frame(frame);
    sp = frame.base_pointer + bm->bound_method->fn->num_locals;
}

Ad_Object *VM::last_popped_stack_element() {
    return stack[sp];
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
    
    if (left->Type() == OBJ_INT && right->Type() == OBJ_INT) {
        int left_val = ((Ad_Integer_Object*)left)->value;
        int right_val = ((Ad_Integer_Object*)right)->value;
        
        switch (opcode) {
            case OP_ADD:
                result = new Ad_Integer_Object(left_val + right_val);
                break;
            case OP_SUB:
                result = new Ad_Integer_Object(left_val - right_val);
                break;
            case OP_MULTIPLY:
                result = new Ad_Integer_Object(left_val * right_val);
                break;
            case OP_DIVIDE:
                if (right_val == 0) {
                    std::cerr << "[ VM Error ] Division by zero!" << std::endl;
                    return;
                }
                result = new Ad_Integer_Object(left_val / right_val);
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
