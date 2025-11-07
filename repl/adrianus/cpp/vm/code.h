#ifndef AD_VM_CODE_H
#define AD_VM_CODE_H

#include "definition.h"
#include "instructions.h"
#include <sstream>
#include <iomanip>
#include <map>

int read_uint16(const Instructions& instructions, int offset);
int read_uint8(const Instructions& instructions, int offset);

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