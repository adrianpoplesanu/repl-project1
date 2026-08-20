#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../vm.h"
#include "../bytecode.h"
#include "../opcode.h"
#include "../instructions.h"
#include "../../objects.h"
#include "../objects.h"

// --- Test builtins for OP_CALL / execute_call ---------------------------------

static Ad_Object* vm_test_builtin_add(std::vector<Ad_Object*> args, Environment*, GarbageCollector*) {
    if (args.size() != 2) {
        return nullptr;
    }
    if (args[0]->Type() != OBJ_INT || args[1]->Type() != OBJ_INT) {
        return nullptr;
    }
    int a = static_cast<Ad_Integer_Object*>(args[0])->value;
    int b = static_cast<Ad_Integer_Object*>(args[1])->value;
    return new Ad_Integer_Object(a + b);
}

static Ad_Object* vm_test_builtin_return_99(std::vector<Ad_Object*> args, Environment*, GarbageCollector*) {
    (void)args;
    return new Ad_Integer_Object(99);
}

static Ad_Object* vm_test_builtin_returns_null(std::vector<Ad_Object*> args, Environment*, GarbageCollector*) {
    (void)args;
    return nullptr;
}

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

// Helper function to create OP_ARRAY instruction
std::vector<unsigned char> make_array_instruction(int num_elements) {
    std::vector<unsigned char> instruction;
    instruction.push_back(OP_ARRAY);
    auto operand = encode_uint16(num_elements);
    instruction.insert(instruction.end(), operand.begin(), operand.end());
    return instruction;
}

// Helper function to create OP_HASH instruction
std::vector<unsigned char> make_hash_instruction(int num_elements) {
    std::vector<unsigned char> instruction;
    instruction.push_back(OP_HASH);
    auto operand = encode_uint16(num_elements);
    instruction.insert(instruction.end(), operand.begin(), operand.end());
    return instruction;
}

// OP_CALL: 1-byte operand (number of arguments)
std::vector<unsigned char> make_call_instruction(int num_args) {
    std::vector<unsigned char> instruction;
    instruction.push_back(OP_CALL);
    instruction.push_back(static_cast<unsigned char>(num_args & 0xFF));
    return instruction;
}

std::vector<unsigned char> make_get_property_sym_instruction(int sym_index) {
    std::vector<unsigned char> instruction;
    instruction.push_back(OP_GET_PROPERTY_SYM);
    auto operand = encode_uint16(sym_index);
    instruction.insert(instruction.end(), operand.begin(), operand.end());
    return instruction;
}

std::vector<unsigned char> make_patch_property_sym_instruction(int sym_index) {
    std::vector<unsigned char> instruction;
    instruction.push_back(OP_PATCH_PROPERTY_SYM);
    auto operand = encode_uint16(sym_index);
    instruction.insert(instruction.end(), operand.begin(), operand.end());
    return instruction;
}

std::vector<unsigned char> make_get_property_instruction() {
    return {OP_GET_PROPERTY};
}

std::vector<unsigned char> make_set_property_instruction() {
    return {OP_SET_PROPERTY};
}

// Minimal program so VM::run() executes at least one instruction (see loop condition ip < size - 1)
Bytecode make_minimal_runnable_bytecode() {
    std::vector<unsigned char> ins = {OP_NULL, OP_NULL};
    return make_bytecode(ins, {});
}

void test_vm_constructor() {
    std::cout << "running test_vm_constructor...\n";

    VM vm;

    assert(vm.sp == 0);
    assert(vm.gc == nullptr);
    assert(vm.frames_index == 0);
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
    assert(vm.frames_index == 1);
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

void test_vm_equal_operation_integers() {
    std::cout << "running test_vm_equal_operation_integers...\n";

    VM vm;
    Ad_Integer_Object* const1 = new Ad_Integer_Object(10);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(10);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.push_back(OP_EQUAL);

    std::vector<Ad_Object*> constants = {const1, const2};
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.sp == 1);
    assert(vm.stack[0]->Type() == OBJ_BOOL);
    assert(static_cast<Ad_Boolean_Object*>(vm.stack[0])->value == true);
    std::cout << "✓ VM equal operation integers test passed\n";
}

void test_vm_greater_than_operation() {
    std::cout << "running test_vm_greater_than_operation...\n";

    VM vm;
    Ad_Integer_Object* const1 = new Ad_Integer_Object(11);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(7);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.push_back(OP_GREATERTHAN);

    std::vector<Ad_Object*> constants = {const1, const2};
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.sp == 1);
    assert(vm.stack[0]->Type() == OBJ_BOOL);
    assert(static_cast<Ad_Boolean_Object*>(vm.stack[0])->value == true);
    std::cout << "✓ VM greater-than operation test passed\n";
}

void test_vm_bang_operation() {
    std::cout << "running test_vm_bang_operation...\n";

    VM vm;
    std::vector<unsigned char> instructions;
    instructions.push_back(OP_TRUE);
    instructions.push_back(OP_BANG);

    vm.load(make_bytecode(instructions, {}));
    vm.run();

    assert(vm.sp == 1);
    assert(vm.stack[0]->Type() == OBJ_BOOL);
    assert(static_cast<Ad_Boolean_Object*>(vm.stack[0])->value == false);
    std::cout << "✓ VM bang operation test passed\n";
}

void test_vm_minus_operation_float() {
    std::cout << "running test_vm_minus_operation_float...\n";

    VM vm;
    Ad_Float_Object* const1 = new Ad_Float_Object(2.5f);

    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.push_back(OP_MINUS);

    std::vector<Ad_Object*> constants = {const1};
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.sp == 1);
    assert(vm.stack[0]->Type() == OBJ_FLOAT);
    assert(static_cast<Ad_Float_Object*>(vm.stack[0])->value == -2.5f);
    std::cout << "✓ VM minus operation float test passed\n";
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

void test_vm_string_constant_instruction() {
    std::cout << "running test_vm_string_constant_instruction...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0 (load string constant at index 0)
    Ad_String_Object* constant = new Ad_String_Object("hello world");
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
    assert(result->Type() == OBJ_STRING);
    assert(((Ad_String_Object*)result)->value == "hello world");

    std::cout << "✓ VM string constant instruction test passed\n";
}

void test_vm_multiple_string_constants() {
    std::cout << "running test_vm_multiple_string_constants...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0, OP_CONSTANT 1, OP_CONSTANT 2
    Ad_String_Object* const1 = new Ad_String_Object("hello");
    Ad_String_Object* const2 = new Ad_String_Object("world");
    Ad_String_Object* const3 = new Ad_String_Object("!");

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

    // Check that all three string constants are on the stack
    Ad_Object* result3 = vm.stack[2];
    Ad_Object* result2 = vm.stack[1];
    Ad_Object* result1 = vm.stack[0];

    assert(result1->Type() == OBJ_STRING);
    assert(((Ad_String_Object*)result1)->value == "hello");
    assert(result2->Type() == OBJ_STRING);
    assert(((Ad_String_Object*)result2)->value == "world");
    assert(result3->Type() == OBJ_STRING);
    assert(((Ad_String_Object*)result3)->value == "!");

    std::cout << "✓ VM multiple string constants test passed\n";
}

void test_vm_string_empty_string() {
    std::cout << "running test_vm_string_empty_string...\n";

    VM vm;

    // Create bytecode: OP_CONSTANT 0 (empty string)
    Ad_String_Object* constant = new Ad_String_Object("");
    std::vector<unsigned char> instructions = make_constant_instruction(0);
    std::vector<Ad_Object*> constants = {constant};

    Bytecode bytecode = make_bytecode(instructions, constants);
    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];
    assert(result != nullptr);
    assert(result->Type() == OBJ_STRING);
    assert(((Ad_String_Object*)result)->value == "");

    std::cout << "✓ VM string empty string test passed\n";
}

void test_vm_array_instruction() {
    std::cout << "running test_vm_array_instruction...\n";

    VM vm;

    // Bytecode: push two constants then build array of 2 elements
    Ad_Integer_Object* const1 = new Ad_Integer_Object(10);
    Ad_Integer_Object* const2 = new Ad_Integer_Object(20);
    std::vector<unsigned char> instructions;
    auto inst1 = make_constant_instruction(0);
    auto inst2 = make_constant_instruction(1);
    auto inst_array = make_array_instruction(2);
    instructions.insert(instructions.end(), inst1.begin(), inst1.end());
    instructions.insert(instructions.end(), inst2.begin(), inst2.end());
    instructions.insert(instructions.end(), inst_array.begin(), inst_array.end());

    std::vector<Ad_Object*> constants = {const1, const2};
    Bytecode bytecode = make_bytecode(instructions, constants);
    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];
    assert(result != nullptr);
    assert(result->Type() == OBJ_LIST);
    Ad_List_Object* list_obj = (Ad_List_Object*)result;
    assert(list_obj->elements.size() == 2);
    assert(list_obj->elements[0]->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)list_obj->elements[0])->value == 10);
    assert(list_obj->elements[1]->Type() == OBJ_INT);
    assert(((Ad_Integer_Object*)list_obj->elements[1])->value == 20);

    std::cout << "✓ VM array instruction test passed\n";
}

void test_vm_array_empty() {
    std::cout << "running test_vm_array_empty...\n";

    VM vm;

    std::vector<unsigned char> instructions = make_array_instruction(0);
    Bytecode bytecode = make_bytecode(instructions, {});
    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];
    assert(result != nullptr);
    assert(result->Type() == OBJ_LIST);
    Ad_List_Object* list_obj = (Ad_List_Object*)result;
    assert(list_obj->elements.size() == 0);

    std::cout << "✓ VM array empty test passed\n";
}

void test_vm_hash_empty() {
    std::cout << "running test_vm_hash_empty...\n";

    VM vm;

    std::vector<unsigned char> instructions = make_hash_instruction(0);
    Bytecode bytecode = make_bytecode(instructions, {});
    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];
    assert(result != nullptr);
    assert(result->Type() == OBJ_HASH);
    Ad_Hash_Object* hash_obj = (Ad_Hash_Object*)result;
    assert(hash_obj->pairs.empty());

    std::cout << "✓ VM hash empty test passed\n";
}

void test_vm_hash_instruction() {
    std::cout << "running test_vm_hash_instruction...\n";

    VM vm;

    // Bytecode: push key 1, value 10, key 2, value 20, then build hash of 4 elements (2 pairs)
    Ad_Integer_Object* key_1 = new Ad_Integer_Object(1);
    Ad_Integer_Object* val_10 = new Ad_Integer_Object(10);
    Ad_Integer_Object* key_2 = new Ad_Integer_Object(2);
    Ad_Integer_Object* val_20 = new Ad_Integer_Object(20);

    std::vector<unsigned char> instructions;
    auto c0 = make_constant_instruction(0);
    auto c1 = make_constant_instruction(1);
    auto c2 = make_constant_instruction(2);
    auto c3 = make_constant_instruction(3);
    auto h4 = make_hash_instruction(4);
    instructions.insert(instructions.end(), c0.begin(), c0.end());
    instructions.insert(instructions.end(), c1.begin(), c1.end());
    instructions.insert(instructions.end(), c2.begin(), c2.end());
    instructions.insert(instructions.end(), c3.begin(), c3.end());
    instructions.insert(instructions.end(), h4.begin(), h4.end());

    std::vector<Ad_Object*> constants = {key_1, val_10, key_2, val_20};
    Bytecode bytecode = make_bytecode(instructions, constants);
    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];
    assert(result != nullptr);
    assert(result->Type() == OBJ_HASH);
    Ad_Hash_Object* hash_obj = (Ad_Hash_Object*)result;
    assert(hash_obj->pairs.size() == 2);

    for (auto& kv : hash_obj->pairs) {
        const HashPair& p = kv.second;
        Ad_Object* k = p.key;
        Ad_Object* v = p.value;
        assert(k != nullptr && v != nullptr);
        assert(k->Type() == OBJ_INT);
        assert(v->Type() == OBJ_INT);
        int key_int = ((Ad_Integer_Object*)k)->value;
        int val_int = ((Ad_Integer_Object*)v)->value;
        assert((key_int == 1 && val_int == 10) || (key_int == 2 && val_int == 20));
    }

    std::cout << "✓ VM hash instruction test passed\n";
}

void test_vm_hash_string_keys() {
    std::cout << "running test_vm_hash_string_keys...\n";

    VM vm;

    // Bytecode: push key "a", value 1, key "b", value 2, then build hash of 4 elements (2 pairs)
    Ad_String_Object* key_a = new Ad_String_Object("a");
    Ad_Integer_Object* val_1 = new Ad_Integer_Object(1);
    Ad_String_Object* key_b = new Ad_String_Object("b");
    Ad_Integer_Object* val_2 = new Ad_Integer_Object(2);

    std::vector<unsigned char> instructions;
    auto c0 = make_constant_instruction(0);
    auto c1 = make_constant_instruction(1);
    auto c2 = make_constant_instruction(2);
    auto c3 = make_constant_instruction(3);
    auto h4 = make_hash_instruction(4);
    instructions.insert(instructions.end(), c0.begin(), c0.end());
    instructions.insert(instructions.end(), c1.begin(), c1.end());
    instructions.insert(instructions.end(), c2.begin(), c2.end());
    instructions.insert(instructions.end(), c3.begin(), c3.end());
    instructions.insert(instructions.end(), h4.begin(), h4.end());

    std::vector<Ad_Object*> constants = {key_a, val_1, key_b, val_2};
    Bytecode bytecode = make_bytecode(instructions, constants);
    vm.load(bytecode);
    vm.run();

    assert(vm.sp == 1);
    Ad_Object* result = vm.stack[0];
    assert(result != nullptr);
    assert(result->Type() == OBJ_HASH);
    Ad_Hash_Object* hash_obj = (Ad_Hash_Object*)result;
    assert(hash_obj->pairs.size() == 2);

    for (auto& kv : hash_obj->pairs) {
        const HashPair& p = kv.second;
        Ad_Object* k = p.key;
        Ad_Object* v = p.value;
        assert(k != nullptr && v != nullptr);
        assert(k->Type() == OBJ_STRING);
        assert(v->Type() == OBJ_INT);
        std::string key_str = ((Ad_String_Object*)k)->value;
        int val_int = ((Ad_Integer_Object*)v)->value;
        assert((key_str == "a" && val_int == 1) || (key_str == "b" && val_int == 2));
    }

    std::cout << "✓ VM hash string keys test passed\n";
}

void test_vm_execute_call_builtin_two_args() {
    std::cout << "running test_vm_execute_call_builtin_two_args...\n";

    VM vm;
    vm.load(make_minimal_runnable_bytecode());

    auto* builtin = new Ad_Builtin_Object(vm_test_builtin_add);
    vm.push(builtin);
    vm.push(new Ad_Integer_Object(3));
    vm.push(new Ad_Integer_Object(4));
    vm.execute_call(2);

    assert(vm.frames_index == 1);
    assert(vm.sp == 1);
    assert(vm.stack[0]->Type() == OBJ_INT);
    assert(static_cast<Ad_Integer_Object*>(vm.stack[0])->value == 7);

    std::cout << "✓ VM execute_call builtin (two args) test passed\n";
}

void test_vm_execute_call_builtin_returns_null_pushes_null_object() {
    std::cout << "running test_vm_execute_call_builtin_returns_null_pushes_null_object...\n";

    VM vm;
    vm.load(make_minimal_runnable_bytecode());

    auto* builtin = new Ad_Builtin_Object(vm_test_builtin_returns_null);
    vm.push(builtin);
    vm.execute_call(0);

    assert(vm.frames_index == 1);
    assert(vm.sp == 1);
    assert(vm.stack[0] == &NULLOBJECT);

    std::cout << "✓ VM execute_call builtin (null result -> NULLOBJECT) test passed\n";
}

void test_vm_execute_call_closure_pushes_frame() {
    std::cout << "running test_vm_execute_call_closure_pushes_frame...\n";

    auto* innerFn = new AdCompiledFunction();
    innerFn->num_parameters = 0;
    innerFn->num_locals = 0;
    auto* innerIns = new Instructions();
    innerIns->bytes = {};
    innerIns->size = 0;
    innerFn->instructions = innerIns;

    auto* closure = new AdClosureObject();
    closure->fn = innerFn;

    VM vm;
    vm.load(make_minimal_runnable_bytecode());
    vm.push(closure);
    vm.execute_call(0);

    assert(vm.frames_index == 2);
    assert(vm.current_frame()->cl == closure);
    assert(vm.sp == 1);

    std::cout << "✓ VM execute_call closure pushes frame test passed\n";
}

void test_vm_execute_call_bound_method_pushes_frame_and_bound_instance() {
    std::cout << "running test_vm_execute_call_bound_method_pushes_frame_and_bound_instance...\n";

    auto* innerFn = new AdCompiledFunction();
    innerFn->num_parameters = 1;
    innerFn->num_locals = 2;
    auto* innerIns = new Instructions();
    innerIns->bytes = {OP_NULL, OP_NULL};
    innerIns->size = 2;
    innerFn->instructions = innerIns;

    auto* closure = new AdClosureObject();
    closure->fn = innerFn;

    auto* instance = new AdCompiledInstance();
    AdBoundMethod bound(instance, closure);

    VM vm;
    vm.load(make_minimal_runnable_bytecode());
    vm.push(&bound);
    vm.push(new Ad_Integer_Object(42));
    vm.execute_call(1);

    assert(vm.frames_index == 2);
    assert(vm.current_frame()->bound_instance == instance);
    assert(vm.current_frame()->cl == closure);
    assert(vm.sp == 3);

    std::cout << "✓ VM execute_call bound method test passed\n";
}

void test_vm_op_call_builtin_via_bytecode() {
    std::cout << "running test_vm_op_call_builtin_via_bytecode...\n";

    auto* builtin = new Ad_Builtin_Object(vm_test_builtin_return_99);
    std::vector<Ad_Object*> constants = {builtin};

    std::vector<unsigned char> instructions;
    auto c0 = make_constant_instruction(0);
    instructions.insert(instructions.end(), c0.begin(), c0.end());
    auto call0 = make_call_instruction(0);
    instructions.insert(instructions.end(), call0.begin(), call0.end());

    VM vm;
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.sp == 1);
    assert(vm.stack[0]->Type() == OBJ_INT);
    assert(static_cast<Ad_Integer_Object*>(vm.stack[0])->value == 99);

    std::cout << "✓ VM OP_CALL builtin via bytecode test passed\n";
}

void test_vm_op_call_builtin_two_args_via_bytecode() {
    std::cout << "running test_vm_op_call_builtin_two_args_via_bytecode...\n";

    auto* builtin = new Ad_Builtin_Object(vm_test_builtin_add);
    Ad_Integer_Object* three = new Ad_Integer_Object(3);
    Ad_Integer_Object* five = new Ad_Integer_Object(5);

    std::vector<Ad_Object*> constants = {builtin, three, five};

    std::vector<unsigned char> instructions;
    for (int i = 0; i < 3; ++i) {
        auto c = make_constant_instruction(i);
        instructions.insert(instructions.end(), c.begin(), c.end());
    }
    auto call2 = make_call_instruction(2);
    instructions.insert(instructions.end(), call2.begin(), call2.end());

    VM vm;
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.sp == 1);
    assert(vm.stack[0]->Type() == OBJ_INT);
    assert(static_cast<Ad_Integer_Object*>(vm.stack[0])->value == 8);

    std::cout << "✓ VM OP_CALL builtin two args via bytecode test passed\n";
}

void test_vm_op_call_compiled_class_no_constructor() {
    std::cout << "running test_vm_op_call_compiled_class_no_constructor...\n";

    auto* cls = new AdCompiledClass();
    std::vector<Ad_Object*> constants = {cls};

    std::vector<unsigned char> instructions;
    auto c0 = make_constant_instruction(0);
    instructions.insert(instructions.end(), c0.begin(), c0.end());
    auto call0 = make_call_instruction(0);
    instructions.insert(instructions.end(), call0.begin(), call0.end());

    VM vm;
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.sp == 2);
    assert(vm.stack[0]->Type() == OBJ_COMPILED_CLASS);
    assert(vm.stack[1]->Type() == OBJ_COMPILED_INSTANCE);
    auto* inst = static_cast<AdCompiledInstance*>(vm.stack[1]);
    assert(inst->klass == cls);

    std::cout << "✓ VM OP_CALL compiled class (no constructor) test passed\n";
}

void test_vm_execute_call_invalid_callee() {
    std::cout << "running test_vm_execute_call_invalid_callee...\n";

    VM vm;
    vm.load(make_minimal_runnable_bytecode());
    vm.push(new Ad_Integer_Object(123));
    vm.execute_call(0);

    assert(vm.frames_index == 1);

    std::cout << "✓ VM execute_call invalid callee (non-callable) test passed\n";
}

void test_vm_op_return_value_via_closure_bytecode() {
    std::cout << "running test_vm_op_return_value_via_closure_bytecode...\n";

    auto* innerFn = new AdCompiledFunction();
    innerFn->num_parameters = 0;
    innerFn->num_locals = 0;
    auto* innerIns = new Instructions();
    auto innerConst = make_constant_instruction(1);  // points to integer constant below
    innerIns->bytes.insert(innerIns->bytes.end(), innerConst.begin(), innerConst.end());
    innerIns->bytes.push_back(OP_RETURN_VALUE);
    innerIns->size = innerIns->bytes.size();
    innerFn->instructions = innerIns;

    auto* closure = new AdClosureObject();
    closure->fn = innerFn;
    auto* forty_two = new Ad_Integer_Object(42);

    std::vector<Ad_Object*> constants = {closure, forty_two};
    std::vector<unsigned char> instructions;
    auto c0 = make_constant_instruction(0); // push closure
    instructions.insert(instructions.end(), c0.begin(), c0.end());
    auto call0 = make_call_instruction(0);
    instructions.insert(instructions.end(), call0.begin(), call0.end());

    VM vm;
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.frames_index == 1);
    assert(vm.sp == 1);
    assert(vm.stack[0] != nullptr);
    assert(vm.stack[0]->Type() == OBJ_INT);
    assert(static_cast<Ad_Integer_Object*>(vm.stack[0])->value == 42);

    std::cout << "✓ VM OP_RETURN_VALUE via closure bytecode test passed\n";
}

void test_vm_op_return_via_closure_bytecode() {
    std::cout << "running test_vm_op_return_via_closure_bytecode...\n";

    auto* innerFn = new AdCompiledFunction();
    innerFn->num_parameters = 0;
    innerFn->num_locals = 0;
    auto* innerIns = new Instructions();
    innerIns->bytes = {OP_RETURN};
    innerIns->size = innerIns->bytes.size();
    innerFn->instructions = innerIns;

    auto* closure = new AdClosureObject();
    closure->fn = innerFn;

    std::vector<Ad_Object*> constants = {closure};
    std::vector<unsigned char> instructions;
    auto c0 = make_constant_instruction(0); // push closure
    instructions.insert(instructions.end(), c0.begin(), c0.end());
    auto call0 = make_call_instruction(0);
    instructions.insert(instructions.end(), call0.begin(), call0.end());

    VM vm;
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.frames_index == 1);
    assert(vm.sp == 1);
    assert(vm.stack[0] == &NULLOBJECT);

    std::cout << "✓ VM OP_RETURN via closure bytecode test passed\n";
}

void test_vm_op_patch_and_get_property_sym_in_bound_method() {
    std::cout << "running test_vm_op_patch_and_get_property_sym_in_bound_method...\n";

    auto* klass = new AdCompiledClass();
    auto* instance = new AdCompiledInstance();
    instance->klass = klass;

    Ad_Integer_Object* forty_two = new Ad_Integer_Object(42);
    Ad_String_Object* field_name = new Ad_String_Object("x");

    auto* innerFn = new AdCompiledFunction();
    innerFn->num_parameters = 0;
    innerFn->num_locals = 0;
    auto* innerIns = new Instructions();
    std::vector<unsigned char> inner_bytes;
    auto c42 = make_constant_instruction(0);
    inner_bytes.insert(inner_bytes.end(), c42.begin(), c42.end());
    auto cname = make_constant_instruction(1);
    inner_bytes.insert(inner_bytes.end(), cname.begin(), cname.end());
    auto patch0 = make_patch_property_sym_instruction(0);
    inner_bytes.insert(inner_bytes.end(), patch0.begin(), patch0.end());
    auto cname2 = make_constant_instruction(1);
    inner_bytes.insert(inner_bytes.end(), cname2.begin(), cname2.end());
    auto get0 = make_get_property_sym_instruction(0);
    inner_bytes.insert(inner_bytes.end(), get0.begin(), get0.end());
    inner_bytes.push_back(OP_RETURN_VALUE);
    innerIns->bytes = inner_bytes;
    innerIns->size = static_cast<int>(inner_bytes.size());
    innerFn->instructions = innerIns;

    auto* closure = new AdClosureObject();
    closure->fn = innerFn;
    auto* bound = new AdBoundMethod(instance, closure);

    std::vector<Ad_Object*> constants = {forty_two, field_name, bound};

    std::vector<unsigned char> instructions;
    auto cbound = make_constant_instruction(2);
    instructions.insert(instructions.end(), cbound.begin(), cbound.end());
    auto call0 = make_call_instruction(0);
    instructions.insert(instructions.end(), call0.begin(), call0.end());

    VM vm;
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.frames_index == 1);
    assert(vm.sp == 1);
    assert(vm.stack[0] != nullptr);
    assert(vm.stack[0]->Type() == OBJ_INT);
    assert(static_cast<Ad_Integer_Object*>(vm.stack[0])->value == 42);
    assert(instance->fields.size() == 1);
    assert(instance->fields[0] == forty_two);
    assert(klass->field_name_to_index["x"] == 0);

    std::cout << "✓ VM OP_PATCH/GET_PROPERTY_SYM in bound method test passed\n";
}

void test_vm_op_set_and_get_property_external() {
    std::cout << "running test_vm_op_set_and_get_property_external...\n";

    auto* klass = new AdCompiledClass();
    auto* instance = new AdCompiledInstance();
    instance->klass = klass;

    Ad_Integer_Object* ninety_nine = new Ad_Integer_Object(99);
    Ad_String_Object* field_name = new Ad_String_Object("count");

    std::vector<Ad_Object*> constants = {instance, ninety_nine, field_name, field_name};

    std::vector<unsigned char> instructions;
    auto c0 = make_constant_instruction(0);
    instructions.insert(instructions.end(), c0.begin(), c0.end());
    auto c1 = make_constant_instruction(1);
    instructions.insert(instructions.end(), c1.begin(), c1.end());
    auto c2 = make_constant_instruction(2);
    instructions.insert(instructions.end(), c2.begin(), c2.end());
    instructions.push_back(OP_SET_PROPERTY);
    auto c0b = make_constant_instruction(0);
    instructions.insert(instructions.end(), c0b.begin(), c0b.end());
    auto c3 = make_constant_instruction(3);
    instructions.insert(instructions.end(), c3.begin(), c3.end());
    instructions.push_back(OP_GET_PROPERTY);

    VM vm;
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.sp == 1);
    assert(vm.stack[0] != nullptr);
    assert(vm.stack[0]->Type() == OBJ_INT);
    assert(static_cast<Ad_Integer_Object*>(vm.stack[0])->value == 99);
    assert(instance->fields.size() == 1);
    assert(instance->fields[0] == ninety_nine);
    assert(klass->field_name_to_index["count"] == 0);

    std::cout << "✓ VM OP_SET/GET_PROPERTY external access test passed\n";
}

void test_vm_op_get_property_sym_missing_field_returns_null() {
    std::cout << "running test_vm_op_get_property_sym_missing_field_returns_null...\n";

    auto* klass = new AdCompiledClass();
    auto* instance = new AdCompiledInstance();
    instance->klass = klass;

    Ad_String_Object* field_name = new Ad_String_Object("missing");

    auto* innerFn = new AdCompiledFunction();
    innerFn->num_parameters = 0;
    innerFn->num_locals = 0;
    auto* innerIns = new Instructions();
    std::vector<unsigned char> inner_bytes;
    auto cname = make_constant_instruction(0);
    inner_bytes.insert(inner_bytes.end(), cname.begin(), cname.end());
    auto get0 = make_get_property_sym_instruction(0);
    inner_bytes.insert(inner_bytes.end(), get0.begin(), get0.end());
    inner_bytes.push_back(OP_RETURN_VALUE);
    innerIns->bytes = inner_bytes;
    innerIns->size = static_cast<int>(inner_bytes.size());
    innerFn->instructions = innerIns;

    auto* closure = new AdClosureObject();
    closure->fn = innerFn;
    auto* bound = new AdBoundMethod(instance, closure);

    std::vector<Ad_Object*> constants = {field_name, bound};
    std::vector<unsigned char> instructions;
    auto cbound = make_constant_instruction(1);
    instructions.insert(instructions.end(), cbound.begin(), cbound.end());
    auto call0 = make_call_instruction(0);
    instructions.insert(instructions.end(), call0.begin(), call0.end());

    VM vm;
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.frames_index == 1);
    assert(vm.sp == 1);
    assert(vm.stack[0] == &NULLOBJECT);

    std::cout << "✓ VM OP_GET_PROPERTY_SYM missing field returns null test passed\n";
}

void test_vm_bound_instance_propagates_through_nested_closure() {
    std::cout << "running test_vm_bound_instance_propagates_through_nested_closure...\n";

    auto* klass = new AdCompiledClass();
    auto* instance = new AdCompiledInstance();
    instance->klass = klass;
    instance->fields = {new Ad_Integer_Object(7)};

    Ad_String_Object* field_name = new Ad_String_Object("x");

    auto* leafFn = new AdCompiledFunction();
    leafFn->num_parameters = 0;
    leafFn->num_locals = 0;
    auto* leafIns = new Instructions();
    std::vector<unsigned char> leaf_bytes;
    auto cname = make_constant_instruction(0);
    leaf_bytes.insert(leaf_bytes.end(), cname.begin(), cname.end());
    auto get0 = make_get_property_sym_instruction(0);
    leaf_bytes.insert(leaf_bytes.end(), get0.begin(), get0.end());
    leaf_bytes.push_back(OP_RETURN_VALUE);
    leafIns->bytes = leaf_bytes;
    leafIns->size = static_cast<int>(leaf_bytes.size());
    leafFn->instructions = leafIns;

    auto* leafClosure = new AdClosureObject();
    leafClosure->fn = leafFn;

    auto* outerFn = new AdCompiledFunction();
    outerFn->num_parameters = 0;
    outerFn->num_locals = 1;
    auto* outerIns = new Instructions();
    std::vector<unsigned char> outer_bytes;
    auto cclosure = make_constant_instruction(1);
    outer_bytes.insert(outer_bytes.end(), cclosure.begin(), cclosure.end());
    auto call0 = make_call_instruction(0);
    outer_bytes.insert(outer_bytes.end(), call0.begin(), call0.end());
    outer_bytes.push_back(OP_RETURN_VALUE);
    outerIns->bytes = outer_bytes;
    outerIns->size = static_cast<int>(outer_bytes.size());
    outerFn->instructions = outerIns;

    auto* outerClosure = new AdClosureObject();
    outerClosure->fn = outerFn;

    auto* bound = new AdBoundMethod(instance, outerClosure);

    std::vector<Ad_Object*> constants = {field_name, leafClosure, outerClosure, bound};
    std::vector<unsigned char> instructions;
    auto cbound = make_constant_instruction(3);
    instructions.insert(instructions.end(), cbound.begin(), cbound.end());
    auto main_call0 = make_call_instruction(0);
    instructions.insert(instructions.end(), main_call0.begin(), main_call0.end());

    VM vm;
    vm.load(make_bytecode(instructions, constants));
    vm.run();

    assert(vm.frames_index == 1);
    assert(vm.sp == 1);
    assert(vm.stack[0] != nullptr);
    assert(vm.stack[0]->Type() == OBJ_INT);
    assert(static_cast<Ad_Integer_Object*>(vm.stack[0])->value == 7);

    std::cout << "✓ VM bound_instance propagates through nested closure test passed\n";
}

void run_all_vm_tests() {
    std::cout << "=== Running VM tests ===\n";

#if defined(RUN_M0_PROPERTY_TESTS_ONLY)
    test_vm_op_patch_and_get_property_sym_in_bound_method();
    test_vm_op_set_and_get_property_external();
    test_vm_op_get_property_sym_missing_field_returns_null();
    test_vm_bound_instance_propagates_through_nested_closure();
    std::cout << "=== Milestone 0 property tests passed! ===\n\n";
    return;
#endif

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
    test_vm_equal_operation_integers();
    test_vm_greater_than_operation();
    test_vm_bang_operation();
    test_vm_minus_operation_float();

    // OP_SET_GLOBAL and OP_GET_GLOBAL tests
    test_vm_set_global_instruction();
    test_vm_get_global_instruction();
    test_vm_set_and_get_global();
    test_vm_multiple_global_variables();
    test_vm_get_global_before_set();

    // String literal tests
    test_vm_string_constant_instruction();
    test_vm_multiple_string_constants();
    test_vm_string_empty_string();

    // OP_ARRAY tests
    test_vm_array_instruction();
    test_vm_array_empty();

    // OP_HASH tests
    test_vm_hash_empty();
    test_vm_hash_instruction();
    test_vm_hash_string_keys();

    // OP_CALL / execute_call
    test_vm_execute_call_builtin_two_args();
    test_vm_execute_call_builtin_returns_null_pushes_null_object();
    test_vm_execute_call_closure_pushes_frame();
    test_vm_execute_call_bound_method_pushes_frame_and_bound_instance();
    test_vm_op_call_builtin_via_bytecode();
    test_vm_op_call_builtin_two_args_via_bytecode();
    test_vm_op_call_compiled_class_no_constructor();
    test_vm_execute_call_invalid_callee();
    test_vm_op_return_value_via_closure_bytecode();
    test_vm_op_return_via_closure_bytecode();
    test_vm_op_patch_and_get_property_sym_in_bound_method();
    test_vm_op_set_and_get_property_external();
    test_vm_op_get_property_sym_missing_field_returns_null();
    test_vm_bound_instance_propagates_through_nested_closure();

    std::cout << "=== All VM tests passed! ===\n\n";
}

int main() {
    run_all_vm_tests();
    return 0;
}

