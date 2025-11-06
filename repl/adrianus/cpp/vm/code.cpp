#include "code.h"
#include "opcode.h"
#include <iostream>

Code::Code() {
    definitionsMap.insert(std::make_pair(OP_CONSTANT, new Definition("OpConstant", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_ADD, new Definition("OpAdd", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_SUB, new Definition("OpSub", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_MULTIPLY, new Definition("OpMultiply", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_DIVIDE, new Definition("OpDivide", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_POP, new Definition("OpPop", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_TRUE, new Definition("OpTrue", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_FALSE, new Definition("OpFalse", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_EQUAL, new Definition("OpEqual", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_NOTEQUAL, new Definition("OpNotEqual", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_GREATERTHAN, new Definition("OpGreaterThan", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_GREATERTHAN_EQUAL, new Definition("OpGreaterThanEqual", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_MINUS, new Definition("OpMinus", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_BANG, new Definition("OpBang", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_JUMP_NOT_TRUTHY, new Definition("OpJumpNotTruthy", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_JUMP, new Definition("OpJump", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_NULL, new Definition("OpNull", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_GET_GLOBAL, new Definition("OpGetGlobal", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_SET_GLOBAL, new Definition("OpSetGlobal", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_ARRAY, new Definition("OpArray", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_HASH, new Definition("OpHash", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_INDEX, new Definition("OpIndex", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_CALL, new Definition("OpCall", 1, new int[1] {1})));
    definitionsMap.insert(std::make_pair(OP_RETURN_VALUE, new Definition("OpReturnValue", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_RETURN, new Definition("OpReturn", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_GET_LOCAL, new Definition("OpGetLocal", 1, new int[1] {1})));
    definitionsMap.insert(std::make_pair(OP_SET_LOCAL, new Definition("OpSetLocal", 1, new int[1] {1})));
    definitionsMap.insert(std::make_pair(OP_GET_BUILTIN, new Definition("OpGetBuiltin", 1, new int[1] {1})));
    definitionsMap.insert(std::make_pair(OP_CLOSURE, new Definition("OpClosure", 2, new int[2] {2, 1})));
    definitionsMap.insert(std::make_pair(OP_GET_FREE, new Definition("OpGetFree", 1, new int[1] {1})));
    definitionsMap.insert(std::make_pair(OP_CURRENT_CLOSURE, new Definition("OpCurrentClosure", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_CLASS, new Definition("OpClass", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_SET_METHOD, new Definition("OpSetMethod", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_GET_PROPERTY, new Definition("OpGetProperty", 0, new int))); // TODO: this should have operands
    definitionsMap.insert(std::make_pair(OP_SET_PROPERTY, new Definition("OpSetProperty", 0, new int))); // TODO: this should have operands
    definitionsMap.insert(std::make_pair(OP_SET_PROPERTY_SYM, new Definition("OpSetPropertySym", 1, new int[1] {2}))); // TODO: this might have more operands
    definitionsMap.insert(std::make_pair(OP_GET_PROPERTY_SYM, new Definition("OpGetPropertySym", 1, new int[1] {2}))); // TODO: this might have more operands
    definitionsMap.insert(std::make_pair(OP_PATCH_PROPERTY_SYM, new Definition("OpPatchPropertySym", 1, new int[1] {2}))); // TODO: this might have more operands
}

Code::~Code() {
    for (auto & it : definitionsMap) {
        delete it.second;
    }
}

std::string Code::toString() {
    std::string result;
    int offset = 0;
    
    while (offset < instructions.size) {
        result += disassembleInstruction(offset);
        offset += getInstructionWidth(offset);
        if (offset < instructions.size) {
            result += "\n";
        }
    }
    
    return result;
}

int Code::getInstructionWidth(int offset) {
    if (offset >= instructions.size) return 0;
    
    unsigned char op = instructions.get(offset);
    auto it = definitionsMap.find(op);
    if (it == definitionsMap.end()) return 1; // Unknown opcode, assume 1 byte
    
    Definition* def = it->second;
    int width = 1; // opcode itself
    for (int i = 0; i < def->size; i++) {
        width += def->operandWidths[i];
    }
    return width;
}

int Code::read_uint16(const Instructions& instructions, int offset) {
    if (offset < 0 || offset + 1 >= instructions.size) {
        std::cerr << "Error: failed to process instructions with offset " << offset << " in read_uint16\n";
        return 0;
    }
    int high = static_cast<int>(instructions.bytes[offset]);
    int low = static_cast<int>(instructions.bytes[offset + 1]);
    return (high << 8) | low;
}

int Code::read_uint8(const Instructions& instructions, int offset) {
    if (offset < 0 || offset >= instructions.size) {
        std::cerr << "Error: failed to process instructions with offset " << offset << " in read_uint8\n";
        return 0;
    }
    return static_cast<int>(instructions.bytes[offset]);
}

std::string Code::disassembleInstruction(int offset) {
    std::ostringstream out;
    
    if (offset >= instructions.size) {
        out << "ERROR: offset " << offset << " out of bounds";
        return out.str();
    }
    
    unsigned char op = instructions.get(offset);
    auto it = definitionsMap.find(op);
    
    if (it == definitionsMap.end()) {
        out << "ERROR: unknown opcode " << (int)op << " at offset " << offset;
        return out.str();
    }
    
    Definition* def = it->second;
    out << std::setfill('0') << std::setw(4) << offset << " " << def->name;
    
    int currentOffset = offset + 1;
    
    for (int i = 0; i < def->size; i++) {
        int operandWidth = def->operandWidths[i];
        int operand = 0;
        
        switch (operandWidth) {
            case 1:
                if (currentOffset < instructions.size) {
                    operand = instructions.get(currentOffset);
                }
                break;
            case 2:
                if (currentOffset + 1 < instructions.size) {
                    operand = instructions.get(currentOffset) | 
                             (instructions.get(currentOffset + 1) << 8);
                }
                break;
            default:
                operand = 0;
                break;
        }
        
        out << " " << operand;
        currentOffset += operandWidth;
    }
    
    return out.str();
}
