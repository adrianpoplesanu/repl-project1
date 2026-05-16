#include <iostream>
#include <fstream>
#include "repl.h"
#include "parser.cpp"
#include "evaluator.cpp"
#include "environment.cpp"
#include "bootstrap.cpp"
#include "thread_workers.h"
#include "profiling.h"
#include "task_scheduler.cpp"
#include <thread>
#include <cstdlib>
#include <cstdint>

#define SHOW_RESIDUAL_GC_OBJECTS 0

namespace {
void run_vm_with_optional_instruction_limit(VM& vm) {
    uint64_t max_ins = 0;
    if (const char* e = std::getenv("AD_VM_MAX_INSTRUCTIONS")) {
        max_ins = std::strtoull(e, nullptr, 10);
    }
    vm.run(max_ins);
}
} // namespace

Repl::Repl() {
    garbageCollector = new GarbageCollector();
    env = nullptr;
    evaluator.setGarbageCollector(garbageCollector);
    compiler.gc = garbageCollector;
    vm.gc = garbageCollector;
    task_scheduler_ = std::make_shared<TaskScheduler>(0);
    if (const char* q = std::getenv("ADLANG_QUANTUM_BUDGET")) {
        try {
            size_t budget = static_cast<size_t>(std::stoull(q));
            if (budget > 0) {
                task_scheduler_->set_quantum_budget(budget);
            }
        } catch (...) {
            std::cerr << "warning: invalid ADLANG_QUANTUM_BUDGET, using default\n";
        }
    }
    evaluator.setTaskScheduler(task_scheduler_);
    ad_set_global_task_scheduler(task_scheduler_);
}

Repl::~Repl() {
    ad_set_global_task_scheduler(nullptr);
    evaluator.setTaskScheduler(nullptr);
    task_scheduler_.reset();
    if (env != nullptr) {
        delete env; // this could be a regular env that gets deleted by the garbage collector mechanism
        env = nullptr;
    }
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
        bool end_singal = ExecuteLine(line);
        if (end_singal) {
            break;
        }
    }
    evaluator.GarbageCollectEnvironments();
    if (SHOW_RESIDUAL_GC_OBJECTS) {
        memoryProfiling.showTotalResidualGCObjects(evaluator.garbageCollector);
    }
    evaluator.garbageCollector->forceFreeObjects(); // TODO: maybe have a wrapper in evaluator for this
    free_builtin_map();
}

void Repl::LoopVM() {
    while (1) {
        std::string line;
        std::cout << ">> ";
        std::getline(std::cin, line);
        bool end_singal = ExecuteLineVM(line);
        if (end_singal) {
            break;
        }
    }
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
    for (int i = 0; i < threadPool.size(); i++) { // TODO: these checks should be done after every evaluated statement
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
    if (SHOW_RESIDUAL_GC_OBJECTS) {
        memoryProfiling.showTotalResidualGCObjects(evaluator.garbageCollector);
    }
    evaluator.garbageCollector->forceFreeObjects(); // TODO: maybe have a wrapper in evaluator for this
    free_builtin_map();
}

void Repl::ExecuteFileVM(std::ifstream &target) {
    //std::cout << "Executing file VM...\n"; // comment this out for now, used for debugging
    if (target.is_open()) {
        std::string line;
        std::string text;
        while (getline(target, line)) {
            text += line + "\n";
        }
        parser.Load(text);
        program.reset();
        parser.ParseProgram(program);

        compiler.reset();
        compiler.compile(&program);
        Bytecode bytecode = compiler.getBytecode();

        vm.load(bytecode);
        vm.printLogs();
        run_vm_with_optional_instruction_limit(vm);

        garbageCollector->unmarkAllObjects();
        garbageCollector->markObjects(vm.stack, vm.sp);
        garbageCollector->sweepObjects();
        // Program output matches `Evaluator::EvalProgram` via OP_FILE_STMT_OUTPUT opcodes.
    } else {
        std::cout << "empty or missing ad source file\n";
        program.reset();
    }
    target.close();
}

bool Repl::ExecuteLine(std::string line) {
    parser.Load(line);
    program.reset();
    parser.ParseProgram(program);

    Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, *env);
    if (res && res->Type() == OBJ_SIGNAL) {
        return true;
    }
    return false;
}

bool Repl::ExecuteLineVM(std::string line) {
    parser.Load(line);
    program.reset();
    parser.ParseProgram(program);
    std::cout << program.ToString();

    compiler.reset();
    compiler.compile(&program);

    Bytecode bytecode = compiler.getBytecode();
    compiler.code.instructions = bytecode.instructions;
    std::cout << compiler.code.toString() << '\n'; // asta pare ca functioneaza cum trebuie

    vm.load(bytecode);
    vm.printLogs();
    run_vm_with_optional_instruction_limit(vm);

    garbageCollector->unmarkAllObjects();
    garbageCollector->markObjects(vm.stack, vm.sp);
    garbageCollector->sweepObjects();

    return false;
}