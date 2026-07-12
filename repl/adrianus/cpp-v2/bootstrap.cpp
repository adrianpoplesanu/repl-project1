#include <iostream>
#include <fstream>
#include <vector>

#include "bootstrap.h"
#include "vm/compiler.h"

#define SKIP_BOOTSTRAP false

std::vector<std::string> bootstrap_files {
    "bootstrap/file.ad",
    "bootstrap/logger.ad",
    "bootstrap/test.ad",
    "bootstrap/mat.ad",
    "bootstrap/string_utils.ad",
    "bootstrap/sys.ad",
    "bootstrap/test2.ad",
    "bootstrap/sock.ad",
    "bootstrap/net_utils.ad",
    "bootstrap/requests.ad"
};

namespace {

std::string read_bootstrap_file(const std::string& path) {
    std::ifstream in(path);
    std::string content;
    std::string line;
    bool first = true;
    while (getline(in, line)) {
        if (!first) {
            content += "\n";
        }
        content += line;
        first = false;
    }
    return content;
}

} // namespace

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
    for (const std::string& path : bootstrap_files) {
        add_bootstrap_code(program, parser, evaluator, env, read_bootstrap_file(path));
    }
}

Environment* load_bootstrap(Ad_AST_Program &program, Parser parser, Evaluator *evaluator) {
    Environment *bootstrap = new Environment();
    bootstrap->ref_count = 1;
    bootstrap->isBootstrapEnvironment = true;
    evaluator->garbageCollector->addEnvironment(bootstrap);
    for (const std::string& path : bootstrap_files) {
        if (SKIP_BOOTSTRAP) {
            break;
        }
        add_bootstrap_code(program, parser, evaluator, *bootstrap, read_bootstrap_file(path));
    }
    return bootstrap;
}

void load_bootstrap_vm(Compiler& compiler, Ad_AST_Program& program, Parser& parser) {
    for (const std::string& path : bootstrap_files) {
        if (SKIP_BOOTSTRAP) {
            break;
        }
        parser.Load(read_bootstrap_file(path));
        program.reset();
        parser.ParseProgram(program);
        compiler.compile(&program);
    }
}
