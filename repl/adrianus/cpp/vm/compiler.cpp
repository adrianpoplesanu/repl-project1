#include "compiler.h"
#include "opcode.h"
#include <cstdarg>
#include <iostream>

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

void Compiler::compile(Ad_AST_Node* node) {
    if (node == nullptr) {
        std::cout << "severe error: node is null" << std::endl;
    } else if (node->type == ST_PROGRAM) {
        Ad_AST_Program* program = (Ad_AST_Program*)node;
        for (Ad_AST_Node* stmt : program->statements) {
            compile(stmt);
        }
    } else if (node->type == ST_EXPRESSION_STATEMENT) {
        Ad_AST_ExpressionStatement* expr_stmt = (Ad_AST_ExpressionStatement*)node;
        if (expr_stmt->expression != nullptr) {
            if (expr_stmt->expression->type == ST_DEF_STATEMENT) {
                // hmmm, dupa multe cautari am gasit fixul asta, ce ciudat mi se pare
                compile(expr_stmt->expression);
            } else if (expr_stmt->expression->type == ST_ASSIGN_STATEMENT) {
                compile(expr_stmt->expression);
            } else if (expr_stmt->expression->type == ST_WHILE_EXPRESSION) {
                compile(expr_stmt->expression);
            } else {
                compile(expr_stmt->expression);
                emit(opPop, 0, {});
            }
        }
    }
    // TODO: add support for other statement types
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

std::pair<int, std::vector<unsigned char>> Compiler::make(OpCode opcode, int n, std::vector<int> args) {
    return code.make(opcode, n, args);
}

//int Compiler::emit(OpCodeType op, int n, std::vector<int> args) {
int Compiler::emit(OpCode opcode, int n, std::vector<int> args) {
    if (args.empty()) {
        args = {};
    }
    
    auto result= code.make(opcode, n, args);
    //auto result = make(op, n, args);
    int size = result.first;
    std::vector<unsigned char> instruction = result.second;
    
    int pos = addInstruction(size, instruction);
    setLastInstruction(opcode, pos);
    return pos;
}

int Compiler::addInstruction(int size, std::vector<unsigned char> instruction) {
    int pos = code.instructions.size;
    
    for (int i = 0; i < size; i++) {
        code.instructions.add(instruction[i]);
    }
    
    return pos;
}

void Compiler::setLastInstruction(OpCode opcode, int pos) {
    // Get the previous instruction from current scope
    EmittedInstruction previous = scopes[scopeIndex].lastInstruction;

    // Create new last instruction
    EmittedInstruction last(opcode.byteCode, pos);

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

