#include <iostream>
#include <string>
#include <cassert>
#include "../code.h"
#include "../opcode.h"

void test_empty_instructions() {
    std::cout << "running test_empty_instructions...\n";
    
    Code code;
    std::string result = code.toString();
    
    assert(result == "");
    std::cout << "✓ Empty instructions test passed\n";
}

void test_single_opcode_no_operands() {
    std::cout << "running test_single_opcode_no_operands...\n";
    
    Code code;
    code.instructions.add(OP_ADD);
    
    std::string result = code.toString();
    std::string expected = "0000 OpAdd";
    
    assert(result == expected);
    std::cout << "✓ Single opcode no operands test passed\n";
}

void test_single_opcode_with_operand() {
    std::cout << "running test_single_opcode_with_operand...\n";
    
    Code code;
    // Add OP_CONSTANT (1 byte) + 2-byte operand (65535 = 0xFFFF)
    code.instructions.add(OP_CONSTANT);
    code.instructions.add(255);  // Low byte
    code.instructions.add(255);  // High byte
    
    std::string result = code.toString();
    std::string expected = "0000 OpConstant 65535";
    
    assert(result == expected);
    std::cout << "✓ Single opcode with operand test passed\n";
}

void test_multiple_instructions() {
    std::cout << "running test_multiple_instructions...\n";
    
    Code code;
    // OP_CONSTANT 42 (3 bytes total)
    code.instructions.add(OP_CONSTANT);
    code.instructions.add(0);    // High byte
    code.instructions.add(42);   // Low byte
    // OP_ADD (1 byte)
    code.instructions.add(OP_ADD);
    // OP_POP (1 byte)
    code.instructions.add(OP_POP);
    
    std::string result = code.toString();
    std::string expected = "0000 OpConstant 42\n0003 OpAdd\n0004 OpPop";
    
    assert(result == expected);
    std::cout << "✓ Multiple instructions test passed\n";
}

void test_jump_instruction() {
    std::cout << "running test_jump_instruction...\n";
    
    Code code;
    // OP_JUMP with 2-byte operand (256 = 0x0100)
    code.instructions.add(OP_JUMP);
    code.instructions.add(1);    // High byte
    code.instructions.add(0);    // Low byte
    
    std::string result = code.toString();
    std::string expected = "0000 OpJump 256";
    
    assert(result == expected);
    std::cout << "✓ Jump instruction test passed\n";
}

void test_closure_instruction() {
    std::cout << "running test_closure_instruction...\n";
    
    Code code;
    // OP_CLOSURE with 2 operands: 2-byte constant index + 1-byte free var count
    code.instructions.add(OP_CLOSURE);
    code.instructions.add(0);    // High byte of constant index
    code.instructions.add(10);   // Low byte of constant index
    code.instructions.add(2);    // Free variable count
    
    std::string result = code.toString();
    std::string expected = "0000 OpClosure 10 2";
    
    assert(result == expected);
    std::cout << "✓ Closure instruction test passed\n";
}

void test_unknown_opcode() {
    std::cout << "running test_unknown_opcode...\n";
    
    Code code;
    code.instructions.add(255);  // Unknown opcode
    
    std::string result = code.toString();
    
    // Should contain error message about unknown opcode
    assert(result.find("ERROR: unknown opcode 255") != std::string::npos);
    assert(result.find("at offset 0") != std::string::npos);
    std::cout << "✓ Unknown opcode test passed\n";
}

void test_out_of_bounds_access() {
    std::cout << "running test_out_of_bounds_access...\n";
    
    Code code;
    // Add one instruction
    code.instructions.add(OP_ADD);
    
    std::string result = code.toString();
    
    // Should not contain any error messages for valid instruction
    assert(result.find("ERROR") == std::string::npos);
    assert(result == "0000 OpAdd");
    std::cout << "✓ Out of bounds access test passed\n";
}

void test_edge_case_operands() {
    std::cout << "running test_edge_case_operands...\n";
    
    Code code;
    // Test with operand values at boundaries
    code.instructions.add(OP_CONSTANT);
    code.instructions.add(0);    // High byte (operand = 0)
    code.instructions.add(0);    // Low byte
    code.instructions.add(OP_CONSTANT);
    code.instructions.add(255);  // High byte (operand = 65535)
    code.instructions.add(255);  // Low byte
    
    std::string result = code.toString();
    std::string expected = "0000 OpConstant 0\n0003 OpConstant 65535";
    
    assert(result == expected);
    std::cout << "✓ Edge case operands test passed\n";
}

void test_make_no_operands() {
    std::cout << "running test_make_no_operands...\n";

    Code code;
    auto result = code.make(OP_ADD, 0, {});

    assert(result.first == 1);
    assert(result.second.size() == 1);
    assert(result.second[0] == OP_ADD);
    std::cout << "✓ Code::make no operands test passed\n";
}

void test_make_single_operand() {
    std::cout << "running test_make_single_operand...\n";

    Code code;
    auto result = code.make(OP_CONSTANT, 1, {42});

    assert(result.first == 3);
    assert(result.second.size() == 3);
    assert(result.second[0] == OP_CONSTANT);
    assert(result.second[1] == 0x00);
    assert(result.second[2] == 0x2A);
    std::cout << "✓ Code::make single operand test passed\n";
}

void test_make_multiple_operands() {
    std::cout << "running test_make_multiple_operands...\n";

    Code code;
    auto result = code.make(OP_CLOSURE, 2, {256, 1});

    assert(result.first == 4);
    assert(result.second.size() == 4);
    assert(result.second[0] == OP_CLOSURE);
    assert(result.second[1] == 0x01);
    assert(result.second[2] == 0x00);
    assert(result.second[3] == 0x01);
    std::cout << "✓ Code::make multiple operands test passed\n";
}

void test_make_invalid_opcode() {
    std::cout << "running test_make_invalid_opcode...\n";

    Code code;
    auto result = code.make(static_cast<OpCodeType>(255), 0, {});

    assert(result.first == 0);
    assert(result.second.empty());
    std::cout << "✓ Code::make invalid opcode test passed\n";
}

void test_make_operand_count_mismatch() {
    std::cout << "running test_make_operand_count_mismatch...\n";

    Code code;
    auto result = code.make(OP_CONSTANT, 0, {});

    assert(result.first == 0);
    assert(result.second.empty());
    std::cout << "✓ Code::make operand count mismatch test passed\n";
}

void test_make_insufficient_operands() {
    std::cout << "running test_make_insufficient_operands...\n";

    Code code;
    auto result = code.make(OP_CONSTANT, 1, {});

    assert(result.first == 0);
    assert(result.second.empty());
    std::cout << "✓ Code::make insufficient operands test passed\n";
}

void test_make_operand_out_of_range_two_bytes() {
    std::cout << "running test_make_operand_out_of_range_two_bytes...\n";

    Code code;
    auto result = code.make(OP_CONSTANT, 1, {65536});

    assert(result.first == 0);
    assert(result.second.empty());
    std::cout << "✓ Code::make operand out of range (2 bytes) test passed\n";
}

void test_make_operand_out_of_range_one_byte() {
    std::cout << "running test_make_operand_out_of_range_one_byte...\n";

    Code code;
    auto result = code.make(OP_CALL, 1, {256});

    assert(result.first == 0);
    assert(result.second.empty());
    std::cout << "✓ Code::make operand out of range (1 byte) test passed\n";
}

void run_all_code_tests() {
    std::cout << "=== Running Code tests ===\n";
    
    test_empty_instructions();
    test_single_opcode_no_operands();
    test_single_opcode_with_operand();
    test_multiple_instructions();
    test_jump_instruction();
    test_closure_instruction();
    test_unknown_opcode();
    test_out_of_bounds_access();
    test_edge_case_operands();

    test_make_no_operands();
    test_make_single_operand();
    test_make_multiple_operands();
    test_make_invalid_opcode();
    test_make_operand_count_mismatch();
    test_make_insufficient_operands();
    test_make_operand_out_of_range_two_bytes();
    test_make_operand_out_of_range_one_byte();
    
    std::cout << "=== All Code tests passed! ===\n\n";
}

int main() {
    run_all_code_tests();
    return 0;
}
