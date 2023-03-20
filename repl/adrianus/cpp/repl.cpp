#include <iostream>
#include <fstream>
#include "repl.h"
#include "parser.cpp"
#include "evaluator.cpp"
#include "environment.cpp"
#include "bootstrap.cpp"

Repl::Repl() {

}

Repl::~Repl() {
    delete env2; // this could be a regular env that gets deleted by the garbage collector mechanism
}

void Repl::Loop() {
    Environment* bootstrap = load_bootstrap(program, parser, &evaluator);
    bootstrap->isBootstrapEnvironment = true;

    env2 = newEnvironment();
    env2->SetBootstrapEnvironment(bootstrap);
    env2->isGlobalEnvironment = true;
    evaluator.garbageCollector.mainEnv = env2;

    IS_CONSOLE_RUN = true;

    while (1) {
        std::string line;
        std::cout << ">> ";
        std::getline(std::cin, line);
        bool end_singal = ParseLine(line);
        if (end_singal) {
            break;
        }
    }
    evaluator.GarbageCollectEnvironments();
    evaluator.garbageCollector.forceFreeObjects(); // TODO: maybe have a wrapper in evaluator for this
    free_builtin_map();
}

void Repl::ExecuteFile(std::ifstream &target) {
    Environment* bootstrap = load_bootstrap(program, parser, &evaluator);
    bootstrap->isBootstrapEnvironment = true;

    env2 = newEnvironment();
    env2->SetBootstrapEnvironment(bootstrap);
    env2->isGlobalEnvironment = true;
    evaluator.garbageCollector.mainEnv = env2;
    if (target.is_open()) {
        std::string line;
        std::string text;
        while (getline(target ,line)) {
            text += line + "\n";
        }
        parser.Load(text);
        program.reset();
        parser.ParseProgram(program);
        Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, *env2); // TODO: asta cicleaza in momentul executiei fisierului la while
        // in python nu cicleaza pentru ca fac .read() care ia tot continutul fisierului o data, poate la fel ar trebui sa fac si aici
        if (res && res->Type() == OBJ_SIGNAL) {
            // TODO: mark and sweep cleanup
            //free_Ad_Object_memory(res);
        }
    } else {
        std::cout << "empty or missing ad source file\n";
        program.reset();
    }
    target.close();
    evaluator.GarbageCollectEnvironments();
    evaluator.garbageCollector.forceFreeObjects(); // TODO: maybe have a wrapper in evaluator for this
    free_builtin_map();
}

bool Repl::ParseLine(std::string line) {
    parser.Load(line);
    program.reset();
    parser.ParseProgram(program);

    Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, *env2);
    if (res && res->Type() == OBJ_SIGNAL) {
        // if res->signal_type == SIGNAL_EXIT, else it's a different signal
        // TODO: mark and sweep cleanup
        //free_Ad_Object_memory(res);
        return true;
    }
    return false;
}