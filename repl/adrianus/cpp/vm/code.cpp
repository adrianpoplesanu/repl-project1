#include "code.h"
#include "opcode.h"
#include <iostream>

std::vector<int> int_to_bytes(int param_int) {
    std::vector<int> bytes(4, 0);
    bytes[0] = (param_int >> 24) & 0xFF;
    bytes[1] = (param_int >> 16) & 0xFF;
    bytes[2] = (param_int >> 8) & 0xFF;
    bytes[3] = param_int & 0xFF;
    return bytes;
}

int read_operands(const Definition& definition, const Instructions& instructions, int start, std::vector<int>& operands) {
    int offset = 0;

    for (int w = 0; w < definition.size; ++w) {
        int width = definition.operandWidths[w];
        if (width == 2) {
            operands.push_back(read_uint16(instructions, start + offset));
        } else if (width == 1) {
            operands.push_back(read_uint8(instructions, start + offset));
        }
        offset += width;
    }

    return offset;
}

std::string format_int(int n) {
    std::ostringstream out;
    out << std::setfill('0') << std::setw(4) << n;
    return out.str();
}

std::string format_instruction(const Definition& definition, const std::vector<int>& operands) {
    int operand_count = definition.size + 1;
    std::ostringstream out;

    if (operand_count == 1) {
        out << definition.name;
    } else if (operand_count == 2 && !operands.empty()) {
        out << definition.name << " " << operands[0];
    } else if (operand_count == 3 && operands.size() >= 2) {
        out << definition.name << " " << operands[0] << " " << operands[1];
    } else {
        std::cout << "unknowm instruction format" << std::endl;
    }

    return out.str();
}

int read_uint16(const Instructions& instructions, int offset) {
    if (offset < 0 || offset + 1 >= instructions.size) {
        std::cerr << "Error: failed to process instructions with offset " << offset << " in read_uint16\n";
        return 0;
    }
    int high = static_cast<int>(instructions.bytes[offset]);
    int low = static_cast<int>(instructions.bytes[offset + 1]);
    return (high << 8) | low;
}

int read_uint8(const Instructions& instructions, int offset) {
    if (offset < 0 || offset >= instructions.size) {
        std::cerr << "Error: failed to process instructions with offset " << offset << " in read_uint8\n";
        return 0;
    }
    return static_cast<int>(instructions.bytes[offset]);
}

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
    std::ostringstream out;
    int offset = 0;

    while (offset < instructions.size) {
        Definition* definition = lookup(instructions.bytes[offset]);
        if (definition == nullptr) {
            out << "ERROR: unknown opcode " << static_cast<int>(instructions.bytes[offset]) << " at offset " << offset;
            break;
        }

        std::vector<int> operands;
        int read = read_operands(*definition, instructions, offset + 1, operands);

        out << format_int(offset) << " " << format_instruction(*definition, operands);

        offset += 1 + read;
        if (offset < instructions.size) {
            out << "\n";
        }
    }

    return out.str();
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

Definition* Code::lookup(unsigned char byteCode) {
    auto it = definitionsMap.find(byteCode);
    if (it != definitionsMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::pair<int, std::vector<unsigned char>> Code::make(OpCodeType opcode, int n, const std::vector<int>& args) {
    Definition* definition = lookup(static_cast<unsigned char>(opcode));
    if (definition == nullptr) {
        std::cerr << "Unknown opcode: " << static_cast<int>(opcode) << std::endl;
        return std::make_pair(0, std::vector<unsigned char>());
    }

    if (n != definition->size) {
        std::cerr << "Operand count mismatch for opcode: " << static_cast<int>(opcode) << std::endl;
        return std::make_pair(0, std::vector<unsigned char>());
    }

    if (args.size() < static_cast<size_t>(n)) {
        std::cerr << "Not enough operands provided for opcode: " << static_cast<int>(opcode) << std::endl;
        return std::make_pair(0, std::vector<unsigned char>());
    }

    int instructionLen = 1;
    for (int i = 0; i < definition->size; ++i) {
        instructionLen += definition->operandWidths[i];
    }

    std::vector<unsigned char> instruction(instructionLen);
    instruction[0] = static_cast<unsigned char>(opcode);

    int offset = 1;
    for (int j = 0; j < n; ++j) {
        int width = definition->operandWidths[j];
        int argument = args[j];

        if (width == 2) {
            if (argument < 0 || argument > 0xFFFF) {
                std::cerr << "Operand out of range for 2-byte width: " << argument << std::endl;
                return std::make_pair(0, std::vector<unsigned char>());
            }
            instruction[offset] = static_cast<unsigned char>((argument >> 8) & 0xFF);
            instruction[offset + 1] = static_cast<unsigned char>(argument & 0xFF);
        } else if (width == 1) {
            if (argument < 0 || argument > 0xFF) {
                std::cerr << "Operand out of range for 1-byte width: " << argument << std::endl;
                return std::make_pair(0, std::vector<unsigned char>());
            }
            instruction[offset] = static_cast<unsigned char>(argument & 0xFF);
        } else {
            std::cerr << "severe error: unknown width " << width << std::endl;
            return std::make_pair(0, std::vector<unsigned char>());
        }

        offset += width;
    }

    return std::make_pair(instructionLen, instruction);
}
