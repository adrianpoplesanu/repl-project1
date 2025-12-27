#ifndef AD_VM_UTILS_H
#define AD_VM_UTILS_H

#include "definition.h"
#include "instructions.h"
#include <map>

Definition* lookup(unsigned char _byte);
std::string disassemble_instructions(const Instructions& instructions);

#endif

