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

Ad_Object* dispatchMemberAccessPerObjectType(Ad_Object* target, Ad_AST_Identifier* member, std::vector<Ad_Object*> args, GarbageCollector* gc) {
    if (target->type == OBJ_STRING) {
        Ad_String_Object* stringObject = (Ad_String_Object*) target;
        if (member->value == "size") {
            return new Ad_Integer_Object(stringObject->value.size());
        }
        if (member->value == "length") {
            return new Ad_Integer_Object(stringObject->value.size());
        }
        if (member->value == "lower") {
            std::string data = stringObject->value;
            std::transform(data.begin(), data.end(), data.begin(),
                [](unsigned char c){ return std::tolower(c); });
            return new Ad_String_Object(data);
        }
        if (member->value == "upper") {
            std::string data = stringObject->value;
            std::transform(data.begin(), data.end(), data.begin(),
                [](unsigned char c){ return std::toupper(c); });
            return new Ad_String_Object(data);
        }
        if (member->value == "split") {
            std::string delim = " ";
            if (args.size() > 0) {
                delim = ((Ad_String_Object*) args.at(0))->value;
            }
            Ad_List_Object* result = new Ad_List_Object();

            std::string str = stringObject->value;

            size_t start = 0;
            size_t end = str.find(delim);

            while (end != std::string::npos) {
                Ad_String_Object *obj = new Ad_String_Object(str.substr(start, end - start));
                gc->addObject(obj);
                result->elements.push_back(obj);
                start = end + delim.length();
                end = str.find(delim, start);
            }

            Ad_String_Object *obj = new Ad_String_Object(str.substr(start));
            gc->addObject(obj);
            result->elements.push_back(obj);

            return result;
        }
    }
    if (target->type == OBJ_LIST) {
        if (member->value == "size") {
            Ad_List_Object* listObject = (Ad_List_Object*) target;
            Ad_Integer_Object* result = new Ad_Integer_Object(listObject->elements.size());
            return result;
        }
        if (member->value == "length") {
            Ad_List_Object* listObject = (Ad_List_Object*) target;
            Ad_Integer_Object* result = new Ad_Integer_Object(listObject->elements.size());
            return result;
        }
    }
    return NULL;
}