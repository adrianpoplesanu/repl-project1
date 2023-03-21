#include "eval_utils.h"

void evalSource(std::string source, Environment *env, GarbageCollector* gc) {
    // TODO: escape the contents of source

    Parser parser;
    Evaluator evaluator;
    Ad_AST_Program program;
    evaluator.setGarbageCollector(gc);

    parser.Load(source);
    program.reset();
    parser.ParseProgram(program);
    Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, *env);
}