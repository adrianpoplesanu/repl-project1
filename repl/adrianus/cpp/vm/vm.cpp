#include "vm.h"
#include "code.h"
#include "objects.h"
#include "../evaluator.h"
#include <iostream>

VM::VM() {
    sp = 0;
    gc = nullptr;
    frames.clear();
    constants.clear();
}

void VM::load(Bytecode bytecode) {
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
    Frame frame(closure, -1, 0, nullptr);
    frames.push_back(frame);
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
        }
    }
}

Ad_Object *VM::last_popped_stack_element() {
    return stack[sp];
}

Frame* VM::current_frame() {
    if (frames.empty()) {
        return nullptr;
    }
    return &frames.back();
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
