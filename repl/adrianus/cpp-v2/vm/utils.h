#ifndef AD_VM_UTILS_H
#define AD_VM_UTILS_H

#include "definition.h"
#include "instructions.h"
#include "bytecode.h"
#include <map>
#include <string>

Definition* lookup(unsigned char _byte);
std::string disassemble_instructions(const Instructions& instructions);
void write_bytecode_log(const Bytecode& bytecode);

#endif

