#include <iostream>
#include <cassert>
#include <vector>
#include "../vm.h"
#include "../bytecode.h"
#include "../opcode.h"
#include "../instructions.h"
#include "../../objects.h"

// Helper function to create a bytecode with instructions and constants
Bytecode make_bytecode(const std::vector<unsigned char>& instructions_bytes,
                       const std::vector<Ad_Object*>& constants) {
    Bytecode bytecode;
    bytecode.instructions.bytes = instructions_bytes;
    bytecode.instructions.size = instructions_bytes.size();
    bytecode.constants = constants;
    return bytecode;
}

// Helper function to encode a 16-bit integer as two bytes (big-endian)
std::vector<unsigned char> encode_uint16(int value) {
    std::vector<unsigned char> bytes;
    bytes.push_back((value >> 8) & 0xFF);  // High byte
    bytes.push_back(value & 0xFF);          // Low byte
    return bytes;
}

// Helper function to create OP_CONSTANT instruction
std::vector<unsigned char> make_constant_instruction(int constant_index) {
    std::vector<unsigned char> instruction;
    instruction.push_back(OP_CONSTANT);
    auto operand = encode_uint16(constant_index);
    instruction.insert(instruction.end(), operand.begin(), operand.end());
    return instruction;
}

void test_vm_constructor() {
    std::cout << "running test_vm_constructor...\n";

    VM vm;

    assert(vm.sp == 0);
    assert(vm.gc == nullptr);
    assert(vm.frames.empty());
    assert(vm.constants.empty());

    std::cout << "✓ VM constructor test passed\n";
}

void test_vm_load() {
    std::cout << "running test_vm_load...\n";

    VM vm;

    // Create a simple bytecode
    Ad_Integer_Object* constant = new Ad_Integer_Object(42);
    std::vector<unsigned char> instructions = make_constant_instruction(0);
    std::vector<Ad_Object*> constants = {constant};

    Bytecode bytecode = make_bytecode(instructions, constants);
    vm.load(bytecode);

    assert(vm.constants.size() == 1);
    assert(vm.frames.size() == 1);
    assert(vm.current_frame() != nullptr);

    std::cout << "✓ VM load test passed\n";
}

void test_vm_constant_instruction() {
    std::cout << "running test_vm_constant_instruction...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0 (load constant at index 0)
    Ad_Integer_Object* constant = new Ad_Integer_Object(42);
    std::vector<unsigned char> instructions = make_constant_instruction(0);
    std::vector<Ad_Object*> constants = {constant};

    Bytecode bytecode = make_bytecode(instructions, constants);
    vm.load(bytecode);

    // Verify frame was created
    assert(vm.current_frame() != nullptr);
    Instructions* ins = vm.current_frame()->instructions();
    assert(ins != nullptr);
    assert(ins->bytes.size() == 3);  // OP_CONSTANT + 2 bytes for operand

    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[vm.sp - 1];  // sp points to next free slot, so last element is at sp-1
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == 42);

    std::cout << "✓ VM constant instruction test passed\n";
}

void test_vm_add_operation() {
    std::cout << "running test_vm_add_operation...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0, OP_CONSTANT 1, OP_ADD
    Ad_Integer_Object* const1 = new Ad_Integer_Object(10);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(20);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.push_back(OP_ADD);

    std::vector<Ad_Object*> constants = {const1, const2};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];  // Use stack directly
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == 30);

    std::cout << "✓ VM add operation test passed\n";
}

void test_vm_subtract_operation() {
    std::cout << "running test_vm_subtract_operation...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0, OP_CONSTANT 1, OP_SUB
    Ad_Integer_Object* const1 = new Ad_Integer_Object(50);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(20);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.push_back(OP_SUB);

    std::vector<Ad_Object*> constants = {const1, const2};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];  // Use stack directly
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == 30);

    std::cout << "✓ VM subtract operation test passed\n";
}

void test_vm_multiply_operation() {
    std::cout << "running test_vm_multiply_operation...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0, OP_CONSTANT 1, OP_MULTIPLY
    Ad_Integer_Object* const1 = new Ad_Integer_Object(6);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(7);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.push_back(OP_MULTIPLY);

    std::vector<Ad_Object*> constants = {const1, const2};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];  // Use stack directly
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == 42);

    std::cout << "✓ VM multiply operation test passed\n";
}

void test_vm_divide_operation() {
    std::cout << "running test_vm_divide_operation...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0, OP_CONSTANT 1, OP_DIVIDE
    Ad_Integer_Object* const1 = new Ad_Integer_Object(20);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(4);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.push_back(OP_DIVIDE);

    std::vector<Ad_Object*> constants = {const1, const2};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];  // Use stack directly
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == 5);

    std::cout << "✓ VM divide operation test passed\n";
}

void test_vm_true_instruction() {
    std::cout << "running test_vm_true_instruction...\n";

    VM vm;

    // Create bytecode: OP_TRUE
    std::vector<unsigned char> instructions;
    instructions.push_back(OP_TRUE);

    std::vector<Ad_Object*> constants;
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];  // Use stack directly
    assert(result != nullptr);
    assert(result->Type() == OBJ_BOOL);
    assert(((Ad_Boolean_Object*)result)->value == true);

    std::cout << "✓ VM true instruction test passed\n";
}

void test_vm_false_instruction() {
    std::cout << "running test_vm_false_instruction...\n";

    VM vm;

    // Create bytecode: OP_FALSE
    std::vector<unsigned char> instructions;
    instructions.push_back(OP_FALSE);

    std::vector<Ad_Object*> constants;
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];  // Use stack directly
    assert(result != nullptr);
    assert(result->Type() == OBJ_BOOL);
    assert(((Ad_Boolean_Object*)result)->value == false);

    std::cout << "✓ VM false instruction test passed\n";
}

void test_vm_pop_instruction() {
    std::cout << "running test_vm_pop_instruction...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0, OP_POP
    Ad_Integer_Object* constant = new Ad_Integer_Object(42);
    std::vector<unsigned char> instructions = make_constant_instruction(0);
    instructions.push_back(OP_POP);

    std::vector<Ad_Object*> constants = {constant};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 0);

    std::cout << "✓ VM pop instruction test passed\n";
}

void test_vm_complex_expression() {
    std::cout << "running test_vm_complex_expression...\n";

    VM vm;

    // Create bytecode: (10 + 20) * 3
    // OP_CONSTANT 0 (10), OP_CONSTANT 1 (20), OP_ADD, OP_CONSTANT 2 (3), OP_MULTIPLY
    Ad_Integer_Object* const1 = new Ad_Integer_Object(10);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(20);
    Ad_Integer_Object* const3 = new Ad_Integer_Object(3);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    auto inst3 = make_constant_instruction(2);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.push_back(OP_ADD);
    instructions.insert(instructions.end(), inst3.begin(), inst3.end());
    instructions.push_back(OP_MULTIPLY);

    std::vector<Ad_Object*> constants = {const1, const2, const3};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];  // Use stack directly
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == 90);  // (10 + 20) * 3 = 90

    std::cout << "✓ VM complex expression test passed\n";
}

void test_vm_negative_numbers() {
    std::cout << "running test_vm_negative_numbers...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0 (-10), OP_CONSTANT 1 (5), OP_SUB
    Ad_Integer_Object* const1 = new Ad_Integer_Object(-10);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(5);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.push_back(OP_SUB);

    std::vector<Ad_Object*> constants = {const1, const2};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];  // Use stack directly
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == -15);  // -10 - 5 = -15

    std::cout << "✓ VM negative numbers test passed\n";
}

void test_vm_zero_division() {
    std::cout << "running test_vm_zero_division...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0 (10), OP_CONSTANT 1 (0), OP_DIVIDE
    Ad_Integer_Object* const1 = new Ad_Integer_Object(10);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(0);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.push_back(OP_DIVIDE);

    std::vector<Ad_Object*> constants = {const1, const2};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    // Division by zero should not push a result, so stack should have the two operands
    // Actually, looking at the code, it pops both operands but doesn't push result on error
    // So stack should be empty after the error
    assert(vm.sp == 0);

    std::cout << "✓ VM zero division test passed\n";
}

void test_vm_empty_instructions() {
    std::cout << "running test_vm_empty_instructions...\n";

    VM vm;

    // Create bytecode with no instructions
    std::vector<unsigned char> instructions;
    std::vector<Ad_Object*> constants;
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 0);

    std::cout << "✓ VM empty instructions test passed\n";
}

void test_vm_multiple_constants() {
    std::cout << "running test_vm_multiple_constants...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0, OP_CONSTANT 1, OP_CONSTANT 2
    Ad_Integer_Object* const1 = new Ad_Integer_Object(1);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(2);
    Ad_Integer_Object* const3 = new Ad_Integer_Object(3);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    auto inst3 = make_constant_instruction(2);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.insert(instructions.end(), inst3.begin(), inst3.end());

    std::vector<Ad_Object*> constants = {const1, const2, const3};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 3);

    // Check that all three constants are on the stack
    Ad_Object* result3 = vm.stack[2];
    Ad_Object* result2 = vm.stack[1];
    Ad_Object* result1 = vm.stack[0];

    assert(result1->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result1)->value == 1);
    assert(result2->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result2)->value == 2);
    assert(result3->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result3)->value == 3);

    std::cout << "✓ VM multiple constants test passed\n";
}

void test_vm_arithmetic_precedence() {
    std::cout << "running test_vm_arithmetic_precedence...\n";

    VM vm;

    // Create bytecode: 2 * 3 + 4 (left-to-right evaluation: (2*3) + 4 = 10)
    // OP_CONSTANT 0 (2), OP_CONSTANT 1 (3), OP_MULTIPLY, OP_CONSTANT 2 (4), OP_ADD
    Ad_Integer_Object* const1 = new Ad_Integer_Object(2);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(3);
    Ad_Integer_Object* const3 = new Ad_Integer_Object(4);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    auto inst3 = make_constant_instruction(2);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.push_back(OP_MULTIPLY);
    instructions.insert(instructions.end(), inst3.begin(), inst3.end());
    instructions.push_back(OP_ADD);

    std::vector<Ad_Object*> constants = {const1, const2, const3};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];  // Use stack directly
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == 10);  // 2 * 3 + 4 = 10

    std::cout << "✓ VM arithmetic precedence test passed\n";
}

void run_all_vm_tests() {
    std::cout << "=== Running VM tests ===\n";

    test_vm_constructor();
    test_vm_load();
    test_vm_constant_instruction();
    test_vm_add_operation();
    test_vm_subtract_operation();
    test_vm_multiply_operation();
    test_vm_divide_operation();
    test_vm_true_instruction();
    test_vm_false_instruction();
    test_vm_pop_instruction();
    test_vm_complex_expression();
    test_vm_negative_numbers();
    test_vm_zero_division();
    test_vm_empty_instructions();
    test_vm_multiple_constants();
    test_vm_arithmetic_precedence();

    std::cout << "=== All VM tests passed! ===\n\n";
}

int main() {
    run_all_vm_tests();
    return 0;
}

