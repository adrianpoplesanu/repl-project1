#include "parser.h"
#include "utils.cpp"

void test_parse_program() {
    Parser parser;
    parser.Load("let a = 1; fn(a,b) { 1+2+3; }");
    Ad_AST_Program program;
    parser.ParseProgram(program);
    print_ast_nodes(&program, 0);
}

void run_parser_tests() {
    test_parse_program();
}