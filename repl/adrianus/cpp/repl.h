#ifndef __REPL_H
#define __REPL_H

#include "parser.h"
#include "evaluator.h"
#include "environment.h"
#include "objects.h"

class Repl {
private:
    bool ParseLine(std::string);
public:
    Parser parser;
    Evaluator evaluator;
    Environment env;

    Repl();
    ~Repl();
    void Loop();
};

#endif