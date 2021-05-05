#include <iostream>
#include "tests.cpp"
#include "test_objects.cpp"
#include "repl.cpp"

#define RUN_TESTS 1 // 0 - no test is being run ar startinme, 1 - all tests are run at startime


int main(int argc, char *argv[]) {
	std::cout << "Adrianus programming language interpreter written in C++... v1.0\n";
	if (RUN_TESTS) {
		run_all_tests();
		test_simple_generic_object();
		test_simple_generic_object();
	}
	return 0;
}