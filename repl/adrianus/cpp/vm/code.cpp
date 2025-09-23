#include "code.h"
#include "opcode.h"

Code::Code() {
    definitionsMap.insert(std::make_pair(OP_CONSTANT, new Definition("OpConstant",1, new int[1] {2})));
}

std::string Code::toString() {
    return std::string();
}
