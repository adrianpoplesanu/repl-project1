#ifndef AD_VM_EMITTED_INSTRUCTION_H
#define AD_VM_EMITTED_INSTRUCTION_H

#include "opcode.h"

class EmittedInstruction {
private:
    OpCodeType opcode;
    int position;

public:
    // Constructor with default parameters (similar to Python __init__)
    EmittedInstruction(OpCodeType opcode = OP_CONSTANT, int position = -1);
    
    // Getters
    OpCodeType getOpcode() const;
    int getPosition() const;
    
    // Setters
    void setOpcode(OpCodeType opcode);
    void setPosition(int position);
};

#endif