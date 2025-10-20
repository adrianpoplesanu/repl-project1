#include "compilation_scope.h"

CompilationScope::CompilationScope() 
    : compilationType("") {
}

CompilationScope::CompilationScope(const std::string& type) 
    : compilationType(type) {
}

CompilationScope::CompilationScope(const Instructions& instr) 
    : instructions(instr), compilationType("") {
}

CompilationScope::CompilationScope(const EmittedInstruction& lastInstr) 
    : lastInstruction(lastInstr), compilationType("") {
}

CompilationScope::CompilationScope(const EmittedInstruction& lastInstr, const EmittedInstruction& prevInstr) 
    : lastInstruction(lastInstr), previousInstruction(prevInstr), compilationType("") {
}
