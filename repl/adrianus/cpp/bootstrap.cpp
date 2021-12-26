#include <iostream>
#include <vector>

#include "bootstrap.h"

std::vector<std::string> bootstrap_files {
    "bootstrap/file.ad"
};

void load_bootstrap(Ad_AST_Program program, Parser parser, Evaluator evaluator, Environment& env) {
    //...
}