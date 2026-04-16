#ifndef __REPL_H
#define __REPL_H

#include "parser.h"
#include "evaluator.h"
#include "environment.h"
#include "objects.h"
#include "settings.h"
#include "gc.h"
#include "vm/compiler.h"
#include "vm/bytecode.h"
#include "vm/vm.h"

class Repl {
private:
    bool ExecuteLine(std::string);
    bool ExecuteLineVM(std::string);
public:
    Parser parser;
    Evaluator evaluator;
    GarbageCollector *garbageCollector;
    Environment *env;
    Compiler compiler;
    VM vm;
    Ad_AST_Program program;

    Repl();
    ~Repl();
    void Loop();
    void LoopVM();
    void ExecuteFile(std::ifstream&);
    void ExecuteFileVM(std::ifstream&);
};

#endif