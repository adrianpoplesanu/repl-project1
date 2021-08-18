#include <iostream>
#include "tests.cpp"
#include "test_objects.cpp"
#include "test_parser.cpp"
#include "test_listobject.cpp"
#include "repl.h"
#include "repl.cpp"

// 0 - no test is being run ar startinme, 1 - all tests are run at startime
#define RUN_TESTS 0
// 0 - parser test is not run, 1 - run and print AST structure
#define RUN_PARSER_TESTS 0
#define RUN_FUNCTION_LITERAL_TESTS 0
#define RUN_LISTOBJECT_TESTS 1


int main(int argc, char *argv[]) {
	std::cout << "Ad interpreter [C++]... v1.0\n";
	if (RUN_TESTS) {
		run_all_tests();
		test_simple_generic_object();
		test_simple_generic_object();
	}
	if (RUN_PARSER_TESTS) {
		test_parse_program();
	}
	if (RUN_FUNCTION_LITERAL_TESTS) {
		test_funtion_literal();
	}
	if (RUN_LISTOBJECT_TESTS) {
		test_simple_list_object();
	}
	Repl repl;
	repl.Loop();
	return 0;
}