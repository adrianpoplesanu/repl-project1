#ifndef AD_VM_DEFINITION_H
#define AD_VM_DEFINITION_H

#include <string>

class Definition {
public:
    std::string name;
    int size;
    int *operandWidths;
    bool owns_operand_widths;

    Definition(std::string name, int size, int *operandWidths, bool owns_operand_widths = true);
    ~Definition();
};

#endif