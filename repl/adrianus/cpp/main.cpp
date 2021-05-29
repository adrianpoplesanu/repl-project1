#include <iostream>
#include "tests.cpp"
#include "test_objects.cpp"
#include "test_parser.cpp"
#include "repl.h"
#include "repl.cpp"

// 0 - no test is being run ar startinme, 1 - all tests are run at startime
#define RUN_TESTS 0
// 0 - parser test is not run, 1 - run and print AST structure
#define RUN_PARSER_TESTS 0


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
	Repl repl;
	repl.Loop();
	return 0;
}