#ifndef AD_VM_CODE_H
#define AD_VM_CODE_H

#include "definition.h"
#include "instructions.h"
#include <sstream>
#include <iomanip>
#include <map>

class Code {
public:
    Instructions instructions;
    std::map<unsigned char, Definition*> definitionsMap;

    Code();
    ~Code();
    std::string toString();

private:
    std::string disassembleInstruction(int offset);
    int getInstructionWidth(int offset);
};

#endif