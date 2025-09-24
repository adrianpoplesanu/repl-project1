#ifndef AD_VM_CODE_H
#define AD_VM_CODE_H

#include "definition.h"

class Code {
public:
    Instructions instructions;
    std::map<unsigned char, Definition*> definitionsMap;

    Code();
    ~Code();
    std::string toString();
};

#endif