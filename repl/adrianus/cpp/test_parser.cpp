#include "parser.h"
#include "utils.cpp"

void test_parse_program() {
    Parser parser;
    //parser.Load("let a = 1; let add = fn(a,b) { 1+2+3; } ; werty; ; 1 + 2 - 3 * 4; add(a, 747); if (3 > 1) { 1001; } else { 1002; } let a = true; let b = false; while (3 < 10) { a = a + 1; }");
    parser.Load("while (3 < 10) { 1; }");
    Ad_AST_Program program;
    parser.ParseProgram(program);
    print_ast_nodes(&program, 0);
}

void run_parser_tests() {
    test_parse_program();
}