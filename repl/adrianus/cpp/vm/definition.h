#ifndef AD_VM_DEFINITION_H
#define AD_VM_DEFINITION_H

class Definition {
public:
    std::string name;
    int size;
    int *operandWidths;

    Definition(std::string name, int size, int *operandWidths);
    ~Definition();
};

#endif