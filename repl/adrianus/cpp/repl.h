#ifndef __REPL_H
#define __REPL_H

#include "parser.h"
#include "evaluator.h"

class Repl {
private:
    void ParseLine(std::string);
public:
    Parser parser;
    Evaluator evaluator;

    Repl();
    ~Repl();
    void Loop();
};

#endif