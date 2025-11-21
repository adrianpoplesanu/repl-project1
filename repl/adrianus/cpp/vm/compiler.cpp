#include "compiler.h"
#include <cstdarg>

Compiler::Compiler() {
    constants.clear();
    gc = nullptr;
    scopeIndex = 0;
    // bytecode, instructions, and scopes are initialized by their default constructors
}

Compiler::Compiler(GarbageCollector* gc) {
    constants.clear();
    this->gc = gc;
    scopeIndex = 0;
    // bytecode, instructions, and scopes are initialized by their default constructors
}

void Compiler::reset() {
    instructions = Instructions();
    bytecode = Bytecode();
}

void Compiler::compile(Ad_AST_Program node) {

}

Bytecode Compiler::getBytecode() {
    Bytecode generatedBytecode;
    generatedBytecode.instructions = code.instructions;
    generatedBytecode.constants = constants;
    return generatedBytecode;
}

Definition* Compiler::lookup(OpCodeType op) {
    return code.lookup(static_cast<unsigned char>(op));
}

std::pair<int, std::vector<unsigned char>> Compiler::make(OpCodeType op, int n, std::vector<int> args) {
    return code.make(op, n, args);
}

int Compiler::emit(OpCodeType op, int n, std::vector<int> args) {
    if (args.empty()) {
        args = {};
    }
    
    auto result= code.make(op, n, args);
    //auto result = make(op, n, args);
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
    // Get the previous instruction from current scope
    EmittedInstruction previous = scopes[scopeIndex].lastInstruction;

    // Create new last instruction
    EmittedInstruction last(op, pos);

    // Update the current scope with previous and last instructions
    scopes[scopeIndex].previousInstruction = previous;
    scopes[scopeIndex].lastInstruction = last;
}

// Constants management implementation
int Compiler::addConstant(Ad_Object* obj) {
    constants.push_back(obj);
    return constants.size() - 1; // Return the index of the added constant
}

Ad_Object* Compiler::getConstant(int index) {
    if (index >= 0 && index < constants.size()) {
        return constants[index];
    }
    return nullptr; // Return nullptr for invalid index
}

Instructions Compiler::currentInstructions() {
    if (scopeIndex >= 0 && scopeIndex < scopes.size()) {
        return scopes[scopeIndex].instructions;
    }
    return Instructions(); // Return empty instructions if scope index is invalid
}

