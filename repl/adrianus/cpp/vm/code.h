#ifndef AD_VM_CODE_H
#define AD_VM_CODE_H

#include "definition.h"
#include "instructions.h"
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>

int read_uint16(const Instructions& instructions, int offset);
int read_uint8(const Instructions& instructions, int offset);
std::vector<int> int_to_bytes(int param_int);
int read_operands(const Definition& definition, const Instructions& instructions, int start, std::vector<int>& operands);
std::string format_int(int n);
std::string format_instruction(const Definition& definition, const std::vector<int>& operands);

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