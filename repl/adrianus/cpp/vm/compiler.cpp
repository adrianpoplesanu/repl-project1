#include "compiler.h"
#include <cstdarg>
#include <iostream>

Compiler::Compiler() {
    //...
}

void Compiler::reset() {
    //...
}

void Compiler::compile(Ad_AST_Program node) {

}

Bytecode Compiler::getBytecode() {
    return Bytecode(); // TODO: fix this
}

Definition* Compiler::lookup(OpCodeType op) {
    auto it = code.definitionsMap.find(static_cast<unsigned char>(op));
    if (it != code.definitionsMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::pair<int, std::vector<unsigned char>> Compiler::make(OpCodeType op, int n, std::vector<int> args) {
    // TODO: rename args to operands
    Definition* def = lookup(op);
    if (!def) {
        std::cerr << "Unknown opcode: " << static_cast<int>(op) << std::endl;
        return std::make_pair(0, std::vector<unsigned char>());
    }
    
    int instruction_len = 1;
    for (int i = 0; i < def->size; i++) {
        instruction_len += def->operandWidths[i];
    }
    
    std::vector<unsigned char> instruction(instruction_len);
    instruction[0] = static_cast<unsigned char>(op);
    
    int offset = 1;
    int i = 0;
    for (int j = 0; j < n; j++) {
        int width = def->operandWidths[i];
        i++;
        
        if (width == 2) {
            int argument = args[j];
            // Big-endian encoding for 2-byte operands
            instruction[offset] = static_cast<unsigned char>((argument >> 8) & 0xFF);
            instruction[offset + 1] = static_cast<unsigned char>(argument & 0xFF);
        } else if (width == 1) {
            int argument = args[j];
            instruction[offset] = static_cast<unsigned char>(argument & 0xFF);
        } else {
            std::cerr << "severe error: unknown width " << width << std::endl;
            return std::make_pair(0, std::vector<unsigned char>());
        }
        offset += width;
    }
    
    return std::make_pair(instruction_len, instruction);
}

int Compiler::emit(OpCodeType op, int n, std::vector<int> args) {
    if (args.empty()) {
        args = {};
    }
    
    auto result = make(op, n, args);
    int size = result.first;
    std::vector<unsigned char> instruction = result.second;
    
    int pos = addInstruction(size, instruction);
    setLastInstruction(op, pos);
    return pos;
}

int Compiler::addInstruction(int size, std::vector<unsigned char> instruction) {
    int pos = code.instructions.size;
    
    for (int i = 0; i < size; i++) {
        code.instructions.add(instruction[i]);
    }
    
    return pos;
}

void Compiler::setLastInstruction(OpCodeType op, int pos) {
    // TODO: Implement last instruction tracking if needed
    // This might be used for jump instruction backpatching or other optimizations
}

