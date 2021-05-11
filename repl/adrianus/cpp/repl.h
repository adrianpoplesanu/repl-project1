#ifndef __REPL_H
#define __REPL_H

#include "parser.h"

class Repl {
private:
    void ParseLine(std::string);
public:
    Parser parser;

    Repl();
    ~Repl();
    void Loop();
};

#endif