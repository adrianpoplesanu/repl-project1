#include "emitted_instruction.h"

// Constructor implementation
EmittedInstruction::EmittedInstruction(OpCodeType opcode, int position) 
    : opcode(opcode), position(position) {
}

// Getter implementations
OpCodeType EmittedInstruction::getOpcode() const {
    return opcode;
}

int EmittedInstruction::getPosition() const {
    return position;
}

// Setter implementations
void EmittedInstruction::setOpcode(OpCodeType opcode) {
    this->opcode = opcode;
}

void EmittedInstruction::setPosition(int position) {
    this->position = position;
}
