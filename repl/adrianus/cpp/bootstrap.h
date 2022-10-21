#include "parser.h"
#include "evaluator.h"
#include "environment.h"

void add_bootstrap_code(Ad_AST_Program&, Parser, Evaluator, Environment&, std::string);
void load_bootstrap(Ad_AST_Program&, Parser, Evaluator, Environment&);
Environment* load_bootstrap(Ad_AST_Program&, Parser, Evaluator);