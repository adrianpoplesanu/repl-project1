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

// Helper function to create OP_SET_GLOBAL instruction
std::vector<unsigned char> make_set_global_instruction(int global_index) {
    std::vector<unsigned char> instruction;
    instruction.push_back(OP_SET_GLOBAL);
    auto operand = encode_uint16(global_index);
    instruction.insert(instruction.end(), operand.begin(), operand.end());
    return instruction;
}

// Helper function to create OP_GET_GLOBAL instruction
std::vector<unsigned char> make_get_global_instruction(int global_index) {
    std::vector<unsigned char> instruction;
    instruction.push_back(OP_GET_GLOBAL);
    auto operand = encode_uint16(global_index);
    instruction.insert(instruction.end(), operand.begin(), operand.end());
    return instruction;
}

void test_vm_set_global_instruction() {
    std::cout << "running test_vm_set_global_instruction...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0 (42), OP_SET_GLOBAL 0
    Ad_Integer_Object* constant = new Ad_Integer_Object(42);
    std::vector<unsigned char> instructions;
    auto const_inst = make_constant_instruction(0);
    auto set_global_inst = make_set_global_instruction(0);
    instructions.insert(instructions.end(), const_inst.begin(), const_inst.end());
    instructions.insert(instructions.end(), set_global_inst.begin(), set_global_inst.end());

    std::vector<Ad_Object*> constants = {constant};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    // Verify stack is empty (value was popped and stored in global)
    assert(vm.sp == 0);

    // Verify global variable was set
    assert(vm.globals.size() == 1);
    assert(vm.globals[0] != nullptr);
    assert(vm.globals[0]->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)vm.globals[0])->value == 42);

    std::cout << "✓ VM set global instruction test passed\n";
}

void test_vm_get_global_instruction() {
    std::cout << "running test_vm_get_global_instruction...\n";

    VM vm;

    // First set a global variable
    Ad_Integer_Object* constant = new Ad_Integer_Object(100);
    std::vector<unsigned char> instructions;
    auto const_inst = make_constant_instruction(0);
    auto set_global_inst = make_set_global_instruction(0);
    instructions.insert(instructions.end(), const_inst.begin(), const_inst.end());
    instructions.insert(instructions.end(), set_global_inst.begin(), set_global_inst.end());

    // Then get it
    auto get_global_inst = make_get_global_instruction(0);
    instructions.insert(instructions.end(), get_global_inst.begin(), get_global_inst.end());

    std::vector<Ad_Object*> constants = {constant};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    // Verify stack has the global value
    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == 100);

    // Verify global variable exists
    assert(vm.globals.size() == 1);
    assert(vm.globals[0] != nullptr);
    assert(((Ad_Integer_Object*)vm.globals[0])->value == 100);

    std::cout << "✓ VM get global instruction test passed\n";
}

void test_vm_set_and_get_global() {
    std::cout << "running test_vm_set_and_get_global...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0 (42), OP_SET_GLOBAL 0, OP_GET_GLOBAL 0
    Ad_Integer_Object* constant = new Ad_Integer_Object(42);
    std::vector<unsigned char> instructions;
    auto const_inst = make_constant_instruction(0);
    auto set_global_inst = make_set_global_instruction(0);
    auto get_global_inst = make_get_global_instruction(0);
    instructions.insert(instructions.end(), const_inst.begin(), const_inst.end());
    instructions.insert(instructions.end(), set_global_inst.begin(), set_global_inst.end());
    instructions.insert(instructions.end(), get_global_inst.begin(), get_global_inst.end());

    std::vector<Ad_Object*> constants = {constant};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    // Verify stack has the global value
    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == 42);

    // Verify global variable was set correctly
    assert(vm.globals.size() == 1);
    assert(vm.globals[0] != nullptr);
    assert(((Ad_Integer_Object*)vm.globals[0])->value == 42);

    std::cout << "✓ VM set and get global test passed\n";
}

void test_vm_multiple_global_variables() {
    std::cout << "running test_vm_multiple_global_variables...\n";

    VM vm;

    // Create bytecode: 
    // OP_CONSTANT 0 (10), OP_SET_GLOBAL 0
    // OP_CONSTANT 1 (20), OP_SET_GLOBAL 1
    // OP_GET_GLOBAL 0, OP_GET_GLOBAL 1, OP_ADD
    Ad_Integer_Object* const1 = new Ad_Integer_Object(10);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(20);

    std::vector<unsigned char> instructions;
    auto const_inst1 = make_constant_instruction(0);
    auto set_global_inst1 = make_set_global_instruction(0);
    auto const_inst2 = make_constant_instruction(1);
    auto set_global_inst2 = make_set_global_instruction(1);
    auto get_global_inst1 = make_get_global_instruction(0);
    auto get_global_inst2 = make_get_global_instruction(1);
    
    instructions.insert(instructions.end(), const_inst1.begin(), const_inst1.end());
    instructions.insert(instructions.end(), set_global_inst1.begin(), set_global_inst1.end());
    instructions.insert(instructions.end(), const_inst2.begin(), const_inst2.end());
    instructions.insert(instructions.end(), set_global_inst2.begin(), set_global_inst2.end());
    instructions.insert(instructions.end(), get_global_inst1.begin(), get_global_inst1.end());
    instructions.insert(instructions.end(), get_global_inst2.begin(), get_global_inst2.end());
    instructions.push_back(OP_ADD);

    std::vector<Ad_Object*> constants = {const1, const2};
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    // Verify stack has the result of addition
    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];
    assert(result != nullptr);
    assert(result->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)result)->value == 30); // 10 + 20 = 30

    // Verify both global variables were set
    assert(vm.globals.size() == 2);
    assert(vm.globals[0] != nullptr);
    assert(((Ad_Integer_Object*)vm.globals[0])->value == 10);
    assert(vm.globals[1] != nullptr);
    assert(((Ad_Integer_Object*)vm.globals[1])->value == 20);

    std::cout << "✓ VM multiple global variables test passed\n";
}

void test_vm_get_global_before_set() {
    std::cout << "running test_vm_get_global_before_set...\n";

    VM vm;

    // Create bytecode: OP_GET_GLOBAL 0 (without setting it first)
    std::vector<unsigned char> instructions;
    auto get_global_inst = make_get_global_instruction(0);
    instructions.insert(instructions.end(), get_global_inst.begin(), get_global_inst.end());

    std::vector<Ad_Object*> constants;
    Bytecode bytecode = make_bytecode(instructions, constants);

    vm.load(bytecode);
    vm.run();

    // The VM should handle this gracefully - it checks bounds and prints an error
    // but doesn't push anything to the stack if the index is out of bounds
    // Stack should be empty since nothing was pushed
    assert(vm.sp == 0);
    assert(vm.globals.empty());

    std::cout << "✓ VM get global before set test passed\n";
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

    // OP_SET_GLOBAL and OP_GET_GLOBAL tests
    test_vm_set_global_instruction();
    test_vm_get_global_instruction();
    test_vm_set_and_get_global();
    test_vm_multiple_global_variables();
    test_vm_get_global_before_set();

    std::cout << "=== All VM tests passed! ===\n\n";
}

int main() {
    run_all_vm_tests();
    return 0;
}

