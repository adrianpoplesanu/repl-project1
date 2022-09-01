#ifndef __REPL_H
#define __REPL_H

#include "parser.h"
#include "evaluator.h"
#include "environment_builtins.h"
#include "environment.h"
#include "objects.h"

class Repl {
private:
    bool ParseLine(std::string);
public:
    Parser parser;
    Evaluator evaluator;
    Environment env;
    Ad_AST_Program program;

    Repl();
    ~Repl();
    void Loop();
    void ExecuteFile(std::ifstream&);
};

#endif