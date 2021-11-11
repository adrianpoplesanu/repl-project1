#include <iostream>
#include <fstream>
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
        bool end_singal = ParseLine(line);
        if (end_singal) {
            break;
        }
    }
    free_builtin_map();
}

void Repl::ExecuteFile(std::ifstream &target) {
    if (target.is_open()) {
        std::string line;
        std::string text;
        while (getline(target ,line)) {
            text += line + "\n";
        }
        parser.Load(text);
        Ad_AST_Program program;
        parser.ParseProgram(program);
        Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, env); // TODO: asta cicleaza in momentul executiei fisierului la while
        // in python nu cicleaza pentru ca fac .read() care ia tot continutul fisierului o data, poate la fel ar trebui sa fac si aici
        if (res && res->Type() == OBJ_SIGNAL) {
            free_Ad_Object_memory(res);
        }
    }
    target.close();
    free_builtin_map();
}

bool Repl::ParseLine(std::string line) {
    parser.Load(line);
    Ad_AST_Program program;
    parser.ParseProgram(program);

    Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, env);
    if (res && res->Type() == OBJ_SIGNAL) {
        // if res->signal_type == SIGNAL_EXIT, else it's a different signal
        free_Ad_Object_memory(res);
        return true;
    }
    return false;
}