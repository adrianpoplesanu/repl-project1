
#include "definition.h"

Definition::Definition(std::string n, int s, int *ow) {
    name = n;
    size = s;
    operandWidths = ow;
}

Definition::~Definition() = default;
