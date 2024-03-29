#include "eval_utils.h"
#include <regex>

// https://replit.com/talk/ask/C-Escape-characters-in-a-string/81443

std::string replaceAll(std::string str, const char* begin, int group) {
    std::regex expr(begin);
    std::smatch sm;
    std::regex_search(str, sm, expr);

    while (sm.size() > 0) {
        std::regex_search(str, sm, expr);
        str = std::regex_replace(str, expr, std::string(sm[group]), std::regex_constants::format_first_only);
    }

    return str;
}


void evalSource(std::string source, Environment *env, GarbageCollector* gc) {
    
    unescapeSource(source);

    Parser parser;
    Evaluator evaluator;
    Ad_AST_Program program;
    evaluator.setGarbageCollector(gc);

    parser.Load(source);
    program.reset();
    parser.ParseProgram(program);
    env->isRunningImportCommand = true;
    Ad_Object* res = evaluator.Eval((Ad_AST_Node *)&program, *env);
    env->isRunningImportCommand = false;
}

void unescapeSource(std::string &source) {
    //source = std::regex_replace(source, std::regex("\\\""), "\""); // this doesn't work
    //source = std::regex_replace(source, std::regex("\\\'"), "\'");
    source = replaceAll(source, R"(\\([\"\']))", 1);
}

std::string readFileContent(std::string path) {
    std::ifstream in(path);
    std::string text = "";
    if (in.is_open() ) {
        char mychar;
        while ( in ) {
            mychar = in.get();
            text += mychar;
        }
    }
    return text;
}

void importSource(std::string path, Environment *env, GarbageCollector* gc) {
    std::string sourceFromFile = readFileContent(path);

    evalSource(sourceFromFile, env, gc);
}