#include <iostream>
#include <fstream>
#include <vector>

#include "bootstrap.h"

#define SKIP_BOOTSTRAP false

std::vector<std::string> bootstrap_files {
    "bootstrap/file.ad",
    "bootstrap/logger.ad",
    "bootstrap/test.ad",
    "bootstrap/mat.ad",
    "bootstrap/string_utils.ad",
    "bootstrap/sys.ad",
    "bootstrap/test2.ad",
    "bootstrap/sock.ad"
};

void add_bootstrap_code(Ad_AST_Program &program, Parser parser, Evaluator *evaluator, Environment& env, std::string source) {
    parser.Load(source);
    program.reset();
    parser.ParseProgram(program);
    Ad_Object* res = evaluator->Eval((Ad_AST_Node *)&program, env);
    if (res && res->Type() == OBJ_SIGNAL) {
        // TODO: mark and sweep cleanup
        //free_Ad_Object_memory(res);
    }
}

void load_bootstrap(Ad_AST_Program &program, Parser parser, Evaluator *evaluator, Environment& env) {
    for (std::vector<std::string>::iterator it = bootstrap_files.begin() ; it != bootstrap_files.end(); ++it) {
        std::ifstream in;
        in.open(*it);
        std::string content = "", line;
        bool first = true;
        while(getline(in, line)) {
            if (!first) content += "\n";
            content += line;
            first = false;
        }
        add_bootstrap_code(program, parser, evaluator, env, content);
    }
}

Environment* load_bootstrap(Ad_AST_Program &program, Parser parser, Evaluator *evaluator) {
    Environment *bootstrap = new Environment();
    bootstrap->ref_count = 1;
    bootstrap->isBootstrapEnvironment = true;
    evaluator->garbageCollector->addEnvironment(bootstrap);
    for (std::vector<std::string>::iterator it = bootstrap_files.begin() ; it != bootstrap_files.end(); ++it) {
        if (SKIP_BOOTSTRAP) {
            break;
        }
        std::ifstream in;
        in.open(*it);
        std::string content = "", line;
        bool first = true;
        while(getline(in, line)) {
            if (!first) content += "\n";
            content += line;
            first = false;
        }
        add_bootstrap_code(program, parser, evaluator, *bootstrap, content);
    }
    return bootstrap;
}