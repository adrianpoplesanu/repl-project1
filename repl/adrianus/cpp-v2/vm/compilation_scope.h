#ifndef AD_VM_COMPILATION_SCOPE_H
#define AD_VM_COMPILATION_SCOPE_H

#include "instructions.h"
#include "emitted_instruction.h"
#include <string>

class CompilationScope {
public:
    Instructions instructions;
    int instruction_start;  // index into code.instructions where this scope's emissions begin
    EmittedInstruction lastInstruction;
    EmittedInstruction previousInstruction;
    std::string compilationType;

    CompilationScope();
    CompilationScope(const std::string& type);
    CompilationScope(const Instructions& instr);
    CompilationScope(const EmittedInstruction& lastInstr);
    CompilationScope(const EmittedInstruction& lastInstr, const EmittedInstruction& prevInstr);
};

#endif