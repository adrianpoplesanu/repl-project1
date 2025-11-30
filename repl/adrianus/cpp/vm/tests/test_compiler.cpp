#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include "../compiler.h"
#include "../opcode.h"
#include "../objects.h"
#include "../../gc.h"
#include "../../ast.h"
#include "../../token.h"

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
    
    // Initialize with a scope to test instruction tracking
    CompilationScope scope;
    compiler.scopes.push_back(scope);
    compiler.scopeIndex = 0;
    
    // Test setting first instruction
    compiler.setLastInstruction(OP_ADD, 0);
    
    // Verify the last instruction was set
    assert(compiler.scopes[0].lastInstruction.getOpcode() == OP_ADD);
    assert(compiler.scopes[0].lastInstruction.getPosition() == 0);
    
    // Test setting another instruction
    compiler.setLastInstruction(OP_CONSTANT, 5);
    
    // Verify the previous instruction was preserved
    assert(compiler.scopes[0].previousInstruction.getOpcode() == OP_ADD);
    assert(compiler.scopes[0].previousInstruction.getPosition() == 0);
    
    // Verify the new last instruction
    assert(compiler.scopes[0].lastInstruction.getOpcode() == OP_CONSTANT);
    assert(compiler.scopes[0].lastInstruction.getPosition() == 5);
    
    // Test with a third instruction to verify chaining
    compiler.setLastInstruction(OP_MULTIPLY, 10);
    
    // Verify the previous instruction chain
    assert(compiler.scopes[0].previousInstruction.getOpcode() == OP_CONSTANT);
    assert(compiler.scopes[0].previousInstruction.getPosition() == 5);
    assert(compiler.scopes[0].lastInstruction.getOpcode() == OP_MULTIPLY);
    assert(compiler.scopes[0].lastInstruction.getPosition() == 10);
    
    std::cout << "✓ Set last instruction test passed\n";
}

void test_compile_if_expression_without_alternative() {
    std::cout << "running test_compile_if_expression_without_alternative...\n";
    
    Compiler compiler;
    
    // Create AST nodes for: if (true) { 10 }
    Token if_token("if", TT_IF);
    Ad_AST_IfExpression* if_expr = new Ad_AST_IfExpression(if_token);
    
    // Condition: true
    Token true_token("true", TT_TRUE);
    Ad_AST_Boolean* condition = new Ad_AST_Boolean(true_token, true);
    if_expr->condition = condition;
    
    // Consequence: 10
    Token int_token("10", TT_INT);
    Ad_AST_Integer* consequence = new Ad_AST_Integer(int_token, 10);
    if_expr->consequence = consequence;
    
    // No alternative
    if_expr->alternative = nullptr;
    
    // Compile the if expression
    compiler.compile(if_expr);
    
    // Expected bytecode structure:
    // 1. OP_TRUE (condition)
    // 2. OP_JUMP_NOT_TRUTHY <offset> (jump to after consequence)
    // 3. OP_CONSTANT <index> (consequence: 10)
    // 4. OP_JUMP <offset> (jump to after alternative)
    // 5. OP_NULL (no alternative case)
    
    Instructions& ins = compiler.code.instructions;
    
    // Check that we have the expected instructions
    assert(ins.get(0) == OP_TRUE); // Condition
    
    // Check for OP_JUMP_NOT_TRUTHY
    assert(ins.get(1) == OP_JUMP_NOT_TRUTHY);
    
    // Check for OP_CONSTANT (consequence)
    int const_pos = 4; // After OP_TRUE (1) + OP_JUMP_NOT_TRUTHY (3)
    assert(ins.get(const_pos) == OP_CONSTANT);
    
    // Check for OP_JUMP after consequence
    int jump_pos = const_pos + 3; // After OP_CONSTANT (3 bytes)
    assert(ins.get(jump_pos) == OP_JUMP);
    
    // Check for OP_NULL at the end (no alternative)
    int null_pos = jump_pos + 3; // After OP_JUMP (3 bytes)
    assert(ins.get(null_pos) == OP_NULL);
    
    // Clean up
    delete if_expr;
    
    std::cout << "✓ Compile if expression without alternative test passed\n";
}

void test_compile_if_expression_with_alternative() {
    std::cout << "running test_compile_if_expression_with_alternative...\n";
    
    Compiler compiler;
    
    // Create AST nodes for: if (false) { 10 } else { 20 }
    Token if_token("if", TT_IF);
    Ad_AST_IfExpression* if_expr = new Ad_AST_IfExpression(if_token);
    
    // Condition: false
    Token false_token("false", TT_FALSE);
    Ad_AST_Boolean* condition = new Ad_AST_Boolean(false_token, false);
    if_expr->condition = condition;
    
    // Consequence: 10
    Token int_token1("10", TT_INT);
    Ad_AST_Integer* consequence = new Ad_AST_Integer(int_token1, 10);
    if_expr->consequence = consequence;
    
    // Alternative: 20
    Token int_token2("20", TT_INT);
    Ad_AST_Integer* alternative = new Ad_AST_Integer(int_token2, 20);
    if_expr->alternative = alternative;
    
    // Compile the if expression
    compiler.compile(if_expr);
    
    // Expected bytecode structure:
    // 1. OP_FALSE (condition)
    // 2. OP_JUMP_NOT_TRUTHY <offset> (jump to after consequence)
    // 3. OP_CONSTANT <index> (consequence: 10)
    // 4. OP_JUMP <offset> (jump to after alternative)
    // 5. OP_CONSTANT <index> (alternative: 20)
    
    Instructions& ins = compiler.code.instructions;
    
    // Check that we have the expected instructions
    assert(ins.get(0) == OP_FALSE); // Condition
    
    // Check for OP_JUMP_NOT_TRUTHY
    assert(ins.get(1) == OP_JUMP_NOT_TRUTHY);
    
    // Check for OP_CONSTANT (consequence)
    int const_pos = 4; // After OP_FALSE (1) + OP_JUMP_NOT_TRUTHY (3)
    assert(ins.get(const_pos) == OP_CONSTANT);
    
    // Check for OP_JUMP after consequence
    int jump_pos = const_pos + 3; // After OP_CONSTANT (3 bytes)
    assert(ins.get(jump_pos) == OP_JUMP);
    
    // Check for OP_CONSTANT (alternative)
    int alt_pos = jump_pos + 3; // After OP_JUMP (3 bytes)
    assert(ins.get(alt_pos) == OP_CONSTANT);
    
    // Clean up
    delete if_expr;
    
    std::cout << "✓ Compile if expression with alternative test passed\n";
}

void test_compile_if_expression_with_integer_condition() {
    std::cout << "running test_compile_if_expression_with_integer_condition...\n";
    
    Compiler compiler;
    
    // Create AST nodes for: if (5) { 10 } else { 20 }
    Token if_token("if", TT_IF);
    Ad_AST_IfExpression* if_expr = new Ad_AST_IfExpression(if_token);
    
    // Condition: 5 (integer)
    Token int_token_cond("5", TT_INT);
    Ad_AST_Integer* condition = new Ad_AST_Integer(int_token_cond, 5);
    if_expr->condition = condition;
    
    // Consequence: 10
    Token int_token1("10", TT_INT);
    Ad_AST_Integer* consequence = new Ad_AST_Integer(int_token1, 10);
    if_expr->consequence = consequence;
    
    // Alternative: 20
    Token int_token2("20", TT_INT);
    Ad_AST_Integer* alternative = new Ad_AST_Integer(int_token2, 20);
    if_expr->alternative = alternative;
    
    // Compile the if expression
    compiler.compile(if_expr);
    
    // Expected bytecode structure:
    // 1. OP_CONSTANT <index> (condition: 5)
    // 2. OP_JUMP_NOT_TRUTHY <offset> (jump to after consequence)
    // 3. OP_CONSTANT <index> (consequence: 10)
    // 4. OP_JUMP <offset> (jump to after alternative)
    // 5. OP_CONSTANT <index> (alternative: 20)
    
    Instructions& ins = compiler.code.instructions;
    
    // Check that we have the expected instructions
    assert(ins.get(0) == OP_CONSTANT); // Condition
    
    // Check for OP_JUMP_NOT_TRUTHY after condition
    int jump_not_truthy_pos = 3; // After OP_CONSTANT (3 bytes)
    assert(ins.get(jump_not_truthy_pos) == OP_JUMP_NOT_TRUTHY);
    
    // Check for OP_CONSTANT (consequence)
    int const_pos = jump_not_truthy_pos + 3; // After OP_JUMP_NOT_TRUTHY (3 bytes)
    assert(ins.get(const_pos) == OP_CONSTANT);
    
    // Check for OP_JUMP after consequence
    int jump_pos = const_pos + 3; // After OP_CONSTANT (3 bytes)
    assert(ins.get(jump_pos) == OP_JUMP);
    
    // Check for OP_CONSTANT (alternative)
    int alt_pos = jump_pos + 3; // After OP_JUMP (3 bytes)
    assert(ins.get(alt_pos) == OP_CONSTANT);
    
    // Clean up
    delete if_expr;
    
    std::cout << "✓ Compile if expression with integer condition test passed\n";
}

void test_compile_if_expression_jump_offsets() {
    std::cout << "running test_compile_if_expression_jump_offsets...\n";
    
    Compiler compiler;
    
    // Create AST nodes for: if (true) { 10 } else { 20 }
    Token if_token("if", TT_IF);
    Ad_AST_IfExpression* if_expr = new Ad_AST_IfExpression(if_token);
    
    // Condition: true
    Token true_token("true", TT_TRUE);
    Ad_AST_Boolean* condition = new Ad_AST_Boolean(true_token, true);
    if_expr->condition = condition;
    
    // Consequence: 10
    Token int_token1("10", TT_INT);
    Ad_AST_Integer* consequence = new Ad_AST_Integer(int_token1, 10);
    if_expr->consequence = consequence;
    
    // Alternative: 20
    Token int_token2("20", TT_INT);
    Ad_AST_Integer* alternative = new Ad_AST_Integer(int_token2, 20);
    if_expr->alternative = alternative;
    
    // Compile the if expression
    compiler.compile(if_expr);
    
    Instructions& ins = compiler.code.instructions;
    
    // Structure:
    // 0: OP_TRUE (1 byte)
    // 1: OP_JUMP_NOT_TRUTHY (1 byte) + offset (2 bytes) = 3 bytes total
    // 4: OP_CONSTANT (1 byte) + constant index (2 bytes) = 3 bytes total
    // 7: OP_JUMP (1 byte) + offset (2 bytes) = 3 bytes total
    // 10: OP_CONSTANT (1 byte) + constant index (2 bytes) = 3 bytes total
    // Total: 13 bytes
    
    // Verify OP_JUMP_NOT_TRUTHY offset is set (not the placeholder 9999)
    // The offset is set to after_consequence_pos, which is the position after OP_JUMP (position 10)
    int jump_not_truthy_pos = 1;
    unsigned char high_byte = ins.get(jump_not_truthy_pos + 1);
    unsigned char low_byte = ins.get(jump_not_truthy_pos + 2);
    int jump_not_truthy_offset = (high_byte << 8) | low_byte;
    assert(jump_not_truthy_offset != 9999); // Should be updated from placeholder
    assert(jump_not_truthy_offset == 10); // Should jump to position 10 (after OP_JUMP, which is after consequence)
    
    // Verify OP_JUMP offset is set (not the placeholder 9999)
    // The offset is set to after_alternative_pos, which is the position after the alternative (position 13)
    int jump_pos = 7;
    high_byte = ins.get(jump_pos + 1);
    low_byte = ins.get(jump_pos + 2);
    int jump_offset = (high_byte << 8) | low_byte;
    assert(jump_offset != 9999); // Should be updated from placeholder
    assert(jump_offset == 13); // Should jump to position 13 (after alternative)
    
    // Verify total instruction size
    assert(ins.size == 13);
    
    // Clean up
    delete if_expr;
    
    std::cout << "✓ Compile if expression jump offsets test passed\n";
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
    
    // ST_IF_EXPRESSION tests
    test_compile_if_expression_without_alternative();
    test_compile_if_expression_with_alternative();
    test_compile_if_expression_with_integer_condition();
    test_compile_if_expression_jump_offsets();
    
    std::cout << "=== All Compiler tests passed! ===\n\n";
}

int main() {
    run_all_compiler_tests();
    return 0;
}
