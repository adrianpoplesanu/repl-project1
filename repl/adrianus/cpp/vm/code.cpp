#include "code.h"
#include "opcode.h"

Code::Code() {
    definitionsMap.insert(std::make_pair(OP_CONSTANT, new Definition("OpConstant", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_ADD, new Definition("OpAdd", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_SUB, new Definition("OpSub", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_MULTIPLY, new Definition("OpMultiply", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_DIVIDE, new Definition("OpDivide", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_POP, new Definition("OpPop", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_TRUE, new Definition("OpTrue", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_FALSE, new Definition("OpFalse", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_EQUAL, new Definition("OpEqual", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_NOTEQUAL, new Definition("OpNotEqual", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_GREATERTHAN, new Definition("OpGreaterThan", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_GREATERTHAN_EQUAL, new Definition("OpGreaterThanEqual", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_MINUS, new Definition("OpMinus", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_BANG, new Definition("OpBang", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_JUMP_NOT_TRUTHY, new Definition("OpJumpNotTruthy", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_JUMP, new Definition("OpJump", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_NULL, new Definition("OpNull", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_GET_GLOBAL, new Definition("OpGetGlobal", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_SET_GLOBAL, new Definition("OpSetGlobal", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_ARRAY, new Definition("OpArray", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_HASH, new Definition("OpHash", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_INDEX, new Definition("OpIndex", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_CALL, new Definition("OpCall", 1, new int[1] {1})));
    definitionsMap.insert(std::make_pair(OP_RETURN_VALUE, new Definition("OpReturnValue", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_RETURN, new Definition("OpReturn", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_GET_LOCAL, new Definition("OpGetLocal", 1, new int[1] {1})));
    definitionsMap.insert(std::make_pair(OP_SET_LOCAL, new Definition("OpSetLocal", 1, new int[1] {1})));
    definitionsMap.insert(std::make_pair(OP_GET_BUILTIN, new Definition("OpGetBuiltin", 1, new int[1] {1})));
    definitionsMap.insert(std::make_pair(OP_CLOSURE, new Definition("OpClosure", 2, new int[2] {2, 1})));
    definitionsMap.insert(std::make_pair(OP_GET_FREE, new Definition("OpGetFree", 1, new int[1] {1})));
    definitionsMap.insert(std::make_pair(OP_CURRENT_CLOSURE, new Definition("OpCurrentClosure", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_CLASS, new Definition("OpClass", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_SET_METHOD, new Definition("OpSetMethod", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_GET_PROPERTY, new Definition("OpGetProperty", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_SET_PROPERTY, new Definition("OpSetProperty", 0, new int)));
    definitionsMap.insert(std::make_pair(OP_SET_PROPERTY_SYM, new Definition("OpSetPropertySym", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_GET_PROPERTY_SYM, new Definition("OpGetPropertySym", 1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_PATCH_PROPERTY_SYM, new Definition("OpPatchPropertySym", 1, new int[1] {2})));
}

Code::~Code() {
    for (auto & it : definitionsMap) {
        delete it.second;
    }
}

std::string Code::toString() {
    return std::string();
}
