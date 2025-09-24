#include "code.h"
#include "opcode.h"

Code::Code() {
    definitionsMap.insert(std::make_pair(OP_CONSTANT, new Definition("OpConstant",1, new int[1] {2})));
    definitionsMap.insert(std::make_pair(OP_ADD, new Definition("OpAdd",0, new int)));
    definitionsMap.insert(std::make_pair(OP_SUB, new Definition("OpSub",0, new int)));
    definitionsMap.insert(std::make_pair(OP_MULTIPLY, new Definition("OpMultiply",0, new int)));
    definitionsMap.insert(std::make_pair(OP_DIVIDE, new Definition("OpDivide",0, new int)));
}

Code::~Code() {
    for (auto & it : definitionsMap) {
        delete it.second;
    }
}

std::string Code::toString() {
    return std::string();
}
