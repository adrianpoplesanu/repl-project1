#ifndef AD_VM_BYTECODE_H
#define AD_VM_BYTECODE_H

#include "instructions.h"
#include "../objects.h"
#include <unordered_set>
#include <string>
#include <vector>

class Bytecode {
public:
    Instructions instructions;
    std::vector<Ad_Object*> constants;
    /// Global slot index → name (for VM `__locals()` parity).
    std::vector<std::string> global_names;
    /// Names defined while compiling bootstrap (excluded from VM `__locals()`).
    std::unordered_set<std::string> bootstrap_global_names;
};

#endif