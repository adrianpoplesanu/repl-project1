#include "eval_utils.h"

void evalSource(std::string source, Environment *env, GarbageCollector* gc) {
    std::cout << "evaluating: " << source << "\n";
    // TODO: escape the contents of source
    // TODO: eval source against env

    Parser parser;
    Evaluator evaluator;
    Ad_AST_Program program;
    evaluator.garbageCollector = *gc;

    std::cout << "aaa\n";
    parser.Load(source);
    std::cout << "bbb\n";
    program.reset();
    std::cout << "ccc\n";
    parser.ParseProgram(program);
    std::cout << "ddd\n";
    Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, *env);
    std::cout << "eee\n";
    GarbageCollector aaa;
    evaluator.garbageCollector = aaa;
}