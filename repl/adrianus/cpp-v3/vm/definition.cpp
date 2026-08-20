
#include "definition.h"

Definition::Definition(std::string n, int s, int *ow, bool owns) {
    name = n;
    size = s;
    operandWidths = ow;
    owns_operand_widths = owns;
}

Definition::~Definition() {
    if (owns_operand_widths && operandWidths != nullptr) {
        delete[] operandWidths;
    }
}
