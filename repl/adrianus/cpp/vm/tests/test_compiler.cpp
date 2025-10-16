#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include "../compiler.h"
#include "../opcode.h"

// Forward declarations to avoid complex dependencies
class GarbageCollector {
public:
    // Minimal implementation for testing
};

void test_compiler_constructor() {
    std::cout << "running test_compiler_constructor...\n";
    
    Compiler compiler;
    
    // Test that compiler is initialized with empty instructions
    assert(compiler.code.instructions.size == 0);
    std::cout << "✓ Compiler constructor test passed\n";
}

void test_lookup_valid_opcode() {
    std::cout << "running test_lookup_valid_opcode...\n";
    
    Compiler compiler;
    
    // Test lookup of valid opcodes
    Definition* def = compiler.lookup(OP_ADD);
    assert(def != nullptr);
    assert(def->name == "OpAdd");
    assert(def->size == 0);
    
    def = compiler.lookup(OP_CONSTANT);
    assert(def != nullptr);
    assert(def->name == "OpConstant");
    assert(def->size == 1);
    assert(def->operandWidths[0] == 2);
    
    std::cout << "✓ Lookup valid opcode test passed\n";
}

void test_lookup_invalid_opcode() {
    std::cout << "running test_lookup_invalid_opcode...\n";
    
    Compiler compiler;
    
    // Test lookup of invalid opcode
    Definition* def = compiler.lookup(static_cast<OpCodeType>(255));
    assert(def == nullptr);
    
    std::cout << "✓ Lookup invalid opcode test passed\n";
}

void test_make_no_operands() {
    std::cout << "running test_make_no_operands...\n";
    
    Compiler compiler;
    
    auto result = compiler.make(OP_ADD, 0, {});
    int length = result.first;
    std::vector<unsigned char> instruction = result.second;
    
    assert(length == 1);
    assert(instruction.size() == 1);
    assert(instruction[0] == OP_ADD);
    
    std::cout << "✓ Make no operands test passed\n";
}

void test_make_single_operand() {
    std::cout << "running test_make_single_operand...\n";
    
    Compiler compiler;
    
    auto result = compiler.make(OP_CONSTANT, 1, {42});
    int length = result.first;
    std::vector<unsigned char> instruction = result.second;
    
    assert(length == 3); // 1 opcode + 2 operand bytes
    assert(instruction.size() == 3);
    assert(instruction[0] == OP_CONSTANT);
    // Big-endian encoding: 42 = 0x002A
    assert(instruction[1] == 0x00); // High byte
    assert(instruction[2] == 0x2A); // Low byte
    
    std::cout << "✓ Make single operand test passed\n";
}

void test_make_multiple_operands() {
    std::cout << "running test_make_multiple_operands...\n";
    
    Compiler compiler;
    
    auto result = compiler.make(OP_CLOSURE, 2, {256, 1});
    int length = result.first;
    std::vector<unsigned char> instruction = result.second;
    
    assert(length == 4); // 1 opcode + 2 + 1 operand bytes
    assert(instruction.size() == 4);
    assert(instruction[0] == OP_CLOSURE);
    // First operand (256): 0x0100 in big-endian
    assert(instruction[1] == 0x01); // High byte of 256
    assert(instruction[2] == 0x00); // Low byte of 256
    // Second operand (1): 1 byte
    assert(instruction[3] == 0x01);
    
    std::cout << "✓ Make multiple operands test passed\n";
}

void test_make_invalid_opcode() {
    std::cout << "running test_make_invalid_opcode...\n";
    
    Compiler compiler;
    
    auto result = compiler.make(static_cast<OpCodeType>(255), 0, {});
    int length = result.first;
    std::vector<unsigned char> instruction = result.second;
    
    assert(length == 0);
    assert(instruction.empty());
    
    std::cout << "✓ Make invalid opcode test passed\n";
}

void test_make_edge_case_operands() {
    std::cout << "running test_make_edge_case_operands...\n";
    
    Compiler compiler;
    
    // Test with maximum 2-byte value (65535)
    auto result = compiler.make(OP_CONSTANT, 1, {65535});
    int length = result.first;
    std::vector<unsigned char> instruction = result.second;
    
    assert(length == 3);
    assert(instruction[0] == OP_CONSTANT);
    // 65535 = 0xFFFF in big-endian
    assert(instruction[1] == 0xFF); // High byte
    assert(instruction[2] == 0xFF); // Low byte
    
    // Test with zero
    result = compiler.make(OP_CONSTANT, 1, {0});
    length = result.first;
    instruction = result.second;
    
    assert(length == 3);
    assert(instruction[1] == 0x00); // High byte
    assert(instruction[2] == 0x00); // Low byte
    
    std::cout << "✓ Make edge case operands test passed\n";
}

void test_add_instruction() {
    std::cout << "running test_add_instruction...\n";
    
    Compiler compiler;
    
    std::vector<unsigned char> instruction = {OP_ADD, OP_CONSTANT, 0x2A};
    int pos = compiler.addInstruction(3, instruction);
    
    assert(pos == 0); // First instruction starts at position 0
    assert(compiler.code.instructions.size == 3);
    assert(compiler.code.instructions.get(0) == OP_ADD);
    assert(compiler.code.instructions.get(1) == OP_CONSTANT);
    assert(compiler.code.instructions.get(2) == 0x2A);
    
    // Add another instruction
    std::vector<unsigned char> instruction2 = {OP_POP};
    int pos2 = compiler.addInstruction(1, instruction2);
    
    assert(pos2 == 3); // Second instruction starts at position 3
    assert(compiler.code.instructions.size == 4);
    assert(compiler.code.instructions.get(3) == OP_POP);
    
    std::cout << "✓ Add instruction test passed\n";
}

void test_emit_no_operands() {
    std::cout << "running test_emit_no_operands...\n";
    
    Compiler compiler;
    
    int pos = compiler.emit(OP_ADD);
    
    assert(pos == 0);
    assert(compiler.code.instructions.size == 1);
    assert(compiler.code.instructions.get(0) == OP_ADD);
    
    std::cout << "✓ Emit no operands test passed\n";
}

void test_emit_single_operand() {
    std::cout << "running test_emit_single_operand...\n";
    
    Compiler compiler;
    
    int pos = compiler.emit(OP_CONSTANT, 1, {42});
    
    assert(pos == 0);
    assert(compiler.code.instructions.size == 3);
    assert(compiler.code.instructions.get(0) == OP_CONSTANT);
    assert(compiler.code.instructions.get(1) == 0x00); // High byte of 42
    assert(compiler.code.instructions.get(2) == 0x2A); // Low byte of 42
    
    std::cout << "✓ Emit single operand test passed\n";
}

void test_emit_multiple_operands() {
    std::cout << "running test_emit_multiple_operands...\n";
    
    Compiler compiler;
    
    int pos = compiler.emit(OP_CLOSURE, 2, {256, 1});
    
    assert(pos == 0);
    assert(compiler.code.instructions.size == 4);
    assert(compiler.code.instructions.get(0) == OP_CLOSURE);
    assert(compiler.code.instructions.get(1) == 0x01); // High byte of 256
    assert(compiler.code.instructions.get(2) == 0x00); // Low byte of 256
    assert(compiler.code.instructions.get(3) == 0x01); // Free var count
    
    std::cout << "✓ Emit multiple operands test passed\n";
}

void test_emit_multiple_instructions() {
    std::cout << "running test_emit_multiple_instructions...\n";
    
    Compiler compiler;
    
    int pos1 = compiler.emit(OP_CONSTANT, 1, {42});
    int pos2 = compiler.emit(OP_CONSTANT, 1, {100});
    int pos3 = compiler.emit(OP_ADD);
    
    assert(pos1 == 0);  // First instruction at position 0
    assert(pos2 == 3);  // Second instruction at position 3 (0+3)
    assert(pos3 == 6);  // Third instruction at position 6 (3+3)
    
    assert(compiler.code.instructions.size == 7);
    
    // Verify first instruction
    assert(compiler.code.instructions.get(0) == OP_CONSTANT);
    assert(compiler.code.instructions.get(1) == 0x00);
    assert(compiler.code.instructions.get(2) == 0x2A);
    
    // Verify second instruction
    assert(compiler.code.instructions.get(3) == OP_CONSTANT);
    assert(compiler.code.instructions.get(4) == 0x00);
    assert(compiler.code.instructions.get(5) == 0x64);
    
    // Verify third instruction
    assert(compiler.code.instructions.get(6) == OP_ADD);
    
    std::cout << "✓ Emit multiple instructions test passed\n";
}

void test_emit_with_default_args() {
    std::cout << "running test_emit_with_default_args...\n";
    
    Compiler compiler;
    
    // Test emit with default parameters (should work like no operands)
    int pos = compiler.emit(OP_POP);
    
    assert(pos == 0);
    assert(compiler.code.instructions.size == 1);
    assert(compiler.code.instructions.get(0) == OP_POP);
    
    std::cout << "✓ Emit with default args test passed\n";
}

void test_emit_jump_instruction() {
    std::cout << "running test_emit_jump_instruction...\n";
    
    Compiler compiler;
    
    int pos = compiler.emit(OP_JUMP, 1, {1000});
    
    assert(pos == 0);
    assert(compiler.code.instructions.size == 3);
    assert(compiler.code.instructions.get(0) == OP_JUMP);
    // 1000 = 0x03E8 in big-endian
    assert(compiler.code.instructions.get(1) == 0x03); // High byte
    assert(compiler.code.instructions.get(2) == 0xE8); // Low byte
    
    std::cout << "✓ Emit jump instruction test passed\n";
}

void test_set_last_instruction() {
    std::cout << "running test_set_last_instruction...\n";
    
    Compiler compiler;
    
    // setLastInstruction is currently a no-op, but should not crash
    compiler.setLastInstruction(OP_ADD, 0);
    compiler.setLastInstruction(OP_CONSTANT, 5);
    
    // If we implement last instruction tracking in the future,
    // we can add more specific assertions here
    
    std::cout << "✓ Set last instruction test passed\n";
}

void run_all_compiler_tests() {
    std::cout << "=== Running Compiler tests ===\n";
    
    test_compiler_constructor();
    test_lookup_valid_opcode();
    test_lookup_invalid_opcode();
    test_make_no_operands();
    test_make_single_operand();
    test_make_multiple_operands();
    test_make_invalid_opcode();
    test_make_edge_case_operands();
    test_add_instruction();
    test_emit_no_operands();
    test_emit_single_operand();
    test_emit_multiple_operands();
    test_emit_multiple_instructions();
    test_emit_with_default_args();
    test_emit_jump_instruction();
    test_set_last_instruction();
    
    std::cout << "=== All Compiler tests passed! ===\n\n";
}

int main() {
    run_all_compiler_tests();
    return 0;
}
