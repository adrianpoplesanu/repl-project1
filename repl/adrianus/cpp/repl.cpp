#include <iostream>
#include <fstream>
#include "repl.h"
#include "parser.cpp"
#include "evaluator.cpp"
#include "environment.cpp"
#include "bootstrap.cpp"
#include "thread_workers.h"
#include <thread>

Repl::Repl() {
    garbageCollector = new GarbageCollector();
    evaluator.setGarbageCollector(garbageCollector);
}

Repl::~Repl() {
    delete env; // this could be a regular env that gets deleted by the garbage collector mechanism
    delete garbageCollector;
}

void Repl::Loop() {
    Environment* bootstrap = load_bootstrap(program, parser, &evaluator);
    bootstrap->isBootstrapEnvironment = true;

    env = newEnvironment();
    env->SetBootstrapEnvironment(bootstrap);
    env->isGlobalEnvironment = true;
    evaluator.garbageCollector->mainEnv = env;

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
    evaluator.garbageCollector->forceFreeObjects(); // TODO: maybe have a wrapper in evaluator for this
    free_builtin_map();
}

void Repl::ExecuteFile(std::ifstream &target) {
    Environment* bootstrap = load_bootstrap(program, parser, &evaluator);
    bootstrap->isBootstrapEnvironment = true;

    env = newEnvironment();
    env->SetBootstrapEnvironment(bootstrap);
    env->isGlobalEnvironment = true;
    evaluator.garbageCollector->mainEnv = env;
    if (target.is_open()) {
        std::string line;
        std::string text;
        while (getline(target ,line)) {
            text += line + "\n";
        }
        parser.Load(text);
        program.reset();
        parser.ParseProgram(program);
        Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, *env); // TODO: asta cicleaza in momentul executiei fisierului la while
        // in python nu cicleaza pentru ca fac .read() care ia tot continutul fisierului o data, poate la fel ar trebui sa fac si aici
        if (res && res->Type() == OBJ_SIGNAL) {
            // no need to do anything because of mark and sweep, here i used to clear up memory
        }
    } else {
        std::cout << "empty or missing ad source file\n";
        program.reset();
    }
    target.close();
    /*while (TOTAL_THREADS_RUNNING > 0) { // i don't like this
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }*/
    for (int i = 0; i < threadPool.size(); i++) {
        //std::cout << "try JOINING\n";
        Ad_Thread_Object *target = (Ad_Thread_Object*) (threadPool.at(i));
        if (target->internal_thread->joinable()) {
            target->internal_thread->join();
            //std::cout << "joined!\n";
        }
        target->internal_gc->forceFreeObjects();
        delete target->internal_gc;
        delete target->internal_thread;
    }
    //std::cout << "Finished joining!!!!!\n";
    evaluator.GarbageCollectEnvironments();
    evaluator.garbageCollector->forceFreeObjects(); // TODO: maybe have a wrapper in evaluator for this
    free_builtin_map();
}

bool Repl::ParseLine(std::string line) {
    parser.Load(line);
    program.reset();
    parser.ParseProgram(program);

    Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, *env);
    if (res && res->Type() == OBJ_SIGNAL) {
        return true;
    }
    return false;
}