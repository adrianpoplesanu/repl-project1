#include <iostream>
#include <fstream>
#include <ctime>
#include "tests.cpp"
#include "test_objects.cpp"
#include "test_parser.cpp"
#include "test_listobject.cpp"
#include "profiling.h"
#include "profiling.cpp"
// #include "lexer.cpp" TODO: remove .cpp imports from other files and fix imports, use only .h
#include "repl.h"
#include "repl.cpp"
#include "settings.cpp"
#include "socket_utils.cpp"
#include "thread_utils.cpp"
#include "thread_workers.cpp"

//#include "vm/bytecode.h"
#include "vm/bytecode.cpp"
//#include "vm/code.h"
#include "vm/code.cpp"
//#include "vm/compilation_scope.h"
#include "vm/compilation_scope.cpp"
//#include "vm/compiler.h"
#include "vm/compiler.cpp"
//#include "vm/definition.h"
#include "vm/definition.cpp"
//#include "vm/emitted_instruction.h"
#include "vm/emitted_instruction.cpp"
//#include "vm/frame.h"
#include "vm/frame.cpp"
//#include "vm/instructions.h"
#include "vm/instructions.cpp"
//#include "vm/objects.h"
#include "vm/objects.cpp"
//#include "vm/opcode.h"
#include "vm/opcode.cpp"
//#include "vm/symbol_table.h"
#include "vm/symbol_table.cpp"
//#include "vm/utils.h"
#include "vm/utils.cpp"
//#include "vm/vm.h"
#include "vm/vm.cpp"

// 0 - no test is being run ar startinme, 1 - all tests are run at startime
#define RUN_TESTS 0
// 0 - parser test is not run, 1 - run and print AST structure
#define RUN_PARSER_TESTS 0
#define RUN_FUNCTION_LITERAL_TESTS 0
#define RUN_LISTOBJECT_TESTS 0
#define RUN_LIST_TESTS 0
#define SHOW_RUNNING_TIME 0

int main(int argc, char *argv[]) {
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
	if (RUN_LIST_TESTS) {
		test_list_parsing();
	}

	if (SHOW_RUNNING_TIME) {
        executionTimeProfiling.start();
	}
	Repl repl;
	if (argc == 1) {
		std::cout << "Ad interpreter [C++]... v4.0.1\n";
	    repl.Loop();
    } else {
        if (argc == 2) {
            if (argv[1] == std::string("-vm")) {
                std::cout << "Ad interpreter [C++][vm]... v4.0.1\n";
                repl.LoopVM();
            } else {
                std::ifstream target(argv[1]);
                repl.ExecuteFile(target);
            }
        } else {
            if (argv[1] == std::string("-vm")) {
                for (int i = 2; i < argc; i++) {
                    std::ifstream target(argv[i]);
                    repl.ExecuteFileVM(target);
                }
            } else {
                for (int i = 1; i < argc; i++) {
                    std::ifstream target(argv[i]);
                    repl.ExecuteFile(target);
                }
            }
        }
	}
	if (SHOW_RUNNING_TIME) {
        executionTimeProfiling.stop();
        executionTimeProfiling.showMetric();
	}
	return 0;
}