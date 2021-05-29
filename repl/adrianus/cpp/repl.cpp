#include <iostream>
#include "repl.h"
#include "parser.cpp"
#include "evaluator.cpp"

Repl::Repl() {

}

Repl::~Repl() {

}

void Repl::Loop() {
    while (1) {
        std::string line;
        std::cout << ">> ";
        std::getline(std::cin, line);
        if (line == "exit()") {
            break;
        }
        ParseLine(line);
    }
}

void Repl::ParseLine(std::string line) {
    parser.Load(line);
    Ad_AST_Program program;
    parser.ParseProgram(program);
    //parser.TestInfixFunction(TT_PLUS);
    //program.ToString();
    Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program);
    delete res; // res is null now because EvalProgram returns NULL
}