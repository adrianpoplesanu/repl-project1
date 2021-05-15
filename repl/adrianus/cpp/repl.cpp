#include <iostream>
#include "repl.h"
#include "parser.cpp"

Repl::Repl() {

}

Repl::~Repl() {

}

void Repl::Loop() {
    while (1) {
        std::string line;
        std::cout << ">> ";
        std::getline(std::cin, line);
        ParseLine(line);
    }
}

void Repl::ParseLine(std::string line) {
    parser.Load(line);
    Ad_AST_Program program;
    parser.ParseProgram(program);
    program.ToString();
}