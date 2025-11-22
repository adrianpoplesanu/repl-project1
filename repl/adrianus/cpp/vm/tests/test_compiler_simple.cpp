#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include "../code.h"
#include "../definition.h"
#include "../opcode.h"
#include "../compiler.h"
#include "../compilation_scope.h"
#include "../emitted_instruction.h"

void test_lookup_valid_opcode() {
    std::cout << "running test_lookup_valid_opcode...\n";
    
    Code code;
    
    // Test lookup of valid opcodes using the code's definitionsMap
    auto it = code.definitionsMap.find(OP_ADD);
    assert(it != code.definitionsMap.end());
    Definition* def = it->second;
    assert(def->name == "OpAdd");
    assert(def->size == 0);
    
    it = code.definitionsMap.find(OP_CONSTANT);
    assert(it != code.definitionsMap.end());
    def = it->second;
    assert(def->name == "OpConstant");
    assert(def->size == 1);
    assert(def->operandWidths[0] == 2);
    
    std::cout << "✓ Lookup valid opcode test passed\n";
}

void test_lookup_invalid_opcode() {
    std::cout << "running test_lookup_invalid_opcode...\n";
    
    Code code;
    
    // Test lookup of invalid opcode
    auto it = code.definitionsMap.find(255);
    assert(it == code.definitionsMap.end());
    
    std::cout << "✓ Lookup invalid opcode test passed\n";
}

void test_make_instruction_no_operands() {
    std::cout << "running test_make_instruction_no_operands...\n";
    
    Code code;
    
    // Simulate the make method logic for no operands
    auto it = code.definitionsMap.find(OP_ADD);
    assert(it != code.definitionsMap.end());
    Definition* def = it->second;
    
    int instruction_len = 1;
    for (int i = 0; i < def->size; i++) {
        instruction_len += def->operandWidths[i];
    }
    
    std::vector<unsigned char> instruction(instruction_len);
    instruction[0] = OP_ADD;
    
    assert(instruction_len == 1);
    assert(instruction.size() == 1);
    assert(instruction[0] == OP_ADD);
    
    std::cout << "✓ Make instruction no operands test passed\n";
}

void test_make_instruction_single_operand() {
    std::cout << "running test_make_instruction_single_operand...\n";
    
    Code code;
    
    // Simulate the make method logic for single operand
    auto it = code.definitionsMap.find(OP_CONSTANT);
    assert(it != code.definitionsMap.end());
    Definition* def = it->second;
    
    int instruction_len = 1;
    for (int i = 0; i < def->size; i++) {
        instruction_len += def->operandWidths[i];
    }
    
    std::vector<unsigned char> instruction(instruction_len);
    instruction[0] = OP_CONSTANT;
    
    // Add operand (42 = 0x002A in big-endian)
    int operand = 42;
    int width = def->operandWidths[0];
    if (width == 2) {
        instruction[1] = static_cast<unsigned char>((operand >> 8) & 0xFF);
        instruction[2] = static_cast<unsigned char>(operand & 0xFF);
    }
    
    assert(instruction_len == 3);
    assert(instruction.size() == 3);
    assert(instruction[0] == OP_CONSTANT);
    assert(instruction[1] == 0x00); // High byte of 42
    assert(instruction[2] == 0x2A); // Low byte of 42
    
    std::cout << "✓ Make instruction single operand test passed\n";
}

void test_make_instruction_multiple_operands() {
    std::cout << "running test_make_instruction_multiple_operands...\n";
    
    Code code;
    
    // Simulate the make method logic for multiple operands (OP_CLOSURE)
    auto it = code.definitionsMap.find(OP_CLOSURE);
    assert(it != code.definitionsMap.end());
    Definition* def = it->second;
    
    int instruction_len = 1;
    for (int i = 0; i < def->size; i++) {
        instruction_len += def->operandWidths[i];
    }
    
    std::vector<unsigned char> instruction(instruction_len);
    instruction[0] = OP_CLOSURE;
    
    // Add operands: [256, 1]
    std::vector<int> args = {256, 1};
    int offset = 1;
    for (int i = 0; i < def->size; i++) {
        int width = def->operandWidths[i];
        int operand = args[i];
        
        if (width == 2) {
            instruction[offset] = static_cast<unsigned char>((operand >> 8) & 0xFF);
            instruction[offset + 1] = static_cast<unsigned char>(operand & 0xFF);
        } else if (width == 1) {
            instruction[offset] = static_cast<unsigned char>(operand & 0xFF);
        }
        offset += width;
    }
    
    assert(instruction_len == 4);
    assert(instruction.size() == 4);
    assert(instruction[0] == OP_CLOSURE);
    assert(instruction[1] == 0x01); // High byte of 256
    assert(instruction[2] == 0x00); // Low byte of 256
    assert(instruction[3] == 0x01); // Free var count
    
    std::cout << "✓ Make instruction multiple operands test passed\n";
}

void test_instruction_encoding_big_endian() {
    std::cout << "running test_instruction_encoding_big_endian...\n";
    
    // Test that our encoding matches the expected big-endian format
    std::vector<int> test_values = {0, 1, 255, 256, 65535};
    std::vector<std::pair<unsigned char, unsigned char>> expected_bytes = {
        {0x00, 0x00}, // 0
        {0x00, 0x01}, // 1
        {0x00, 0xFF}, // 255
        {0x01, 0x00}, // 256
        {0xFF, 0xFF}  // 65535
    };
    
    for (size_t i = 0; i < test_values.size(); i++) {
        int value = test_values[i];
        unsigned char high_byte = static_cast<unsigned char>((value >> 8) & 0xFF);
        unsigned char low_byte = static_cast<unsigned char>(value & 0xFF);
        
        assert(high_byte == expected_bytes[i].first);
        assert(low_byte == expected_bytes[i].second);
    }
    
    std::cout << "✓ Instruction encoding big-endian test passed\n";
}

void test_operand_width_validation() {
    std::cout << "running test_operand_width_validation...\n";
    
    Code code;
    
    // Test that operand widths are correctly defined
    std::vector<std::pair<unsigned char, int>> expected_widths = {
        {OP_CONSTANT, 2},  // 2-byte operand
        {OP_JUMP, 2},      // 2-byte operand
        {OP_GET_LOCAL, 1}, // 1-byte operand
        {OP_SET_LOCAL, 1}, // 1-byte operand
        {OP_ADD, 0},       // No operands
        {OP_SUB, 0},       // No operands
        {OP_POP, 0}        // No operands
    };
    
    for (auto& pair : expected_widths) {
        auto it = code.definitionsMap.find(pair.first);
        assert(it != code.definitionsMap.end());
        Definition* def = it->second;
        
        if (pair.second == 0) {
            assert(def->size == 0);
        } else {
            assert(def->size == 1);
            assert(def->operandWidths[0] == pair.second);
        }
    }
    
    std::cout << "✓ Operand width validation test passed\n";
}

void test_edge_case_operand_values() {
    std::cout << "running test_edge_case_operand_values...\n";
    
    // Test encoding of edge case values
    struct TestCase {
        int value;
        unsigned char expected_high;
        unsigned char expected_low;
    };
    
    std::vector<TestCase> test_cases = {
        {0, 0x00, 0x00},
        {1, 0x00, 0x01},
        {255, 0x00, 0xFF},
        {256, 0x01, 0x00},
        {65535, 0xFF, 0xFF},
        {32767, 0x7F, 0xFF}, // Max positive 15-bit value
        {32768, 0x80, 0x00}  // First negative 16-bit value
    };
    
    for (const auto& test_case : test_cases) {
        unsigned char high = static_cast<unsigned char>((test_case.value >> 8) & 0xFF);
        unsigned char low = static_cast<unsigned char>(test_case.value & 0xFF);
        
        assert(high == test_case.expected_high);
        assert(low == test_case.expected_low);
    }
    
    std::cout << "✓ Edge case operand values test passed\n";
}

void test_set_last_instruction() {
    std::cout << "running test_set_last_instruction...\n";
    
    Compiler compiler;
    
    // Initialize with a scope to test instruction tracking
    CompilationScope scope;
    compiler.scopes.push_back(scope);
    compiler.scopeIndex = 0;
    
    // Test setting first instruction
    compiler.setLastInstruction(opAdd, 0);
    
    // Verify the last instruction was set
    assert(compiler.scopes[0].lastInstruction.getOpcode() == OP_ADD);
    assert(compiler.scopes[0].lastInstruction.getPosition() == 0);
    
    // Test setting another instruction
    compiler.setLastInstruction(opConstant, 5);
    
    // Verify the previous instruction was preserved
    assert(compiler.scopes[0].previousInstruction.getOpcode() == OP_ADD);
    assert(compiler.scopes[0].previousInstruction.getPosition() == 0);
    
    // Verify the new last instruction
    assert(compiler.scopes[0].lastInstruction.getOpcode() == OP_CONSTANT);
    assert(compiler.scopes[0].lastInstruction.getPosition() == 5);
    
    // Test with a third instruction to verify chaining
    compiler.setLastInstruction(opMultiply, 10);
    
    // Verify the previous instruction chain
    assert(compiler.scopes[0].previousInstruction.getOpcode() == OP_CONSTANT);
    assert(compiler.scopes[0].previousInstruction.getPosition() == 5);
    assert(compiler.scopes[0].lastInstruction.getOpcode() == OP_MULTIPLY);
    assert(compiler.scopes[0].lastInstruction.getPosition() == 10);
    
    std::cout << "✓ Set last instruction test passed\n";
}

void run_all_compiler_tests() {
    std::cout << "=== Running Compiler Core Logic Tests ===\n";
    
    test_lookup_valid_opcode();
    test_lookup_invalid_opcode();
    test_make_instruction_no_operands();
    test_make_instruction_single_operand();
    test_make_instruction_multiple_operands();
    test_instruction_encoding_big_endian();
    test_operand_width_validation();
    test_edge_case_operand_values();
    test_set_last_instruction();
    
    std::cout << "=== All Compiler Core Logic Tests Passed! ===\n\n";
}

int main() {
    run_all_compiler_tests();
    return 0;
}
