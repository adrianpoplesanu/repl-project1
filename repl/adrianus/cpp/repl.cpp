#include <iostream>
#include "repl.h"
#include "parser.cpp"
#include "evaluator.cpp"
#include "environment.cpp"

Repl::Repl() {

}

Repl::~Repl() {

}

void Repl::Loop() {
    while (1) {
        std::string line;
        std::cout << ">> ";
        std::getline(std::cin, line);
        /*if (line == "exit()") {
            break;
        }*/
        bool end_singal = ParseLine(line);
        if (end_singal) {
            break;
        }
    }
    free_builtin_map();
}

bool Repl::ParseLine(std::string line) {
    parser.Load(line);
    Ad_AST_Program program;
    parser.ParseProgram(program);
    //parser.TestInfixFunction(TT_PLUS);
    //program.ToString();
    Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, env);
    if (res && res->Type() == OBJ_EXIT) {
        free_Ad_Object_memory(res);
        return true;
    }
    return false;
    //delete res; // res is null now because EvalProgram returns NULL
    //free_Ad_Object_memory(res);
}