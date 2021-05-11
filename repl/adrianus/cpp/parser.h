#ifndef __PARSER_H
#define __PARSER_H

#include "lexer.h"
#include <string>

class Parser {
public:
    std::string source;
    Lexer lexer;
    Parser();
    ~Parser();
    void Load(std::string);
    void BuildStatements();
};

#endif