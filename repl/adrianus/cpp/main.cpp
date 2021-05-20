#include <iostream>
#include "tests.cpp"
#include "test_objects.cpp"
#include "test_parser.cpp"
#include "repl.h"
#include "repl.cpp"

#define RUN_TESTS 0 // 0 - no test is being run ar startinme, 1 - all tests are run at startime
#define RUN_PARSER_TESTS 1


int main(int argc, char *argv[]) {
	std::cout << "Ad interpreter [C++ native]... v1.0\n";
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