#include "compilation_scope.h"

CompilationScope::CompilationScope() 
    : instruction_start(0), compilationType("") {
}

CompilationScope::CompilationScope(const std::string& type) 
    : instruction_start(0), compilationType(type) {
}

CompilationScope::CompilationScope(const Instructions& instr) 
    : instructions(instr), instruction_start(0), compilationType("") {
}

CompilationScope::CompilationScope(const EmittedInstruction& lastInstr) 
    : instruction_start(0), lastInstruction(lastInstr), compilationType("") {
}

CompilationScope::CompilationScope(const EmittedInstruction& lastInstr, const EmittedInstruction& prevInstr) 
    : instruction_start(0), lastInstruction(lastInstr), previousInstruction(prevInstr), compilationType("") {
}
