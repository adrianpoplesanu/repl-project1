CFLAGS = -O3 -finline-functions -funroll-loops -fomit-frame-pointer -g -std=c++17 -I/opt/homebrew/opt/openssl@3/include/
LDFLAGS = -L/opt/homebrew/opt/openssl@3/lib/ -lssl -lcrypto

all:
	g++ $(CFLAGS) main.cpp -c -o main.o
	g++ $(CFLAGS) $(LDFLAGS) main.o -o main

clean:
	find . -type f ! -name "*.h" ! -name "*.cpp" ! -name "README.md" ! -name "Makefile" ! -name "*.*" -delete
	find . -name "*.o" -delete

test_code:
	g++ $(CFLAGS) vm/tests/test_code.cpp vm/code.cpp vm/definition.cpp vm/instructions.cpp vm/opcode.cpp -o test_code $(LDFLAGS)
	./test_code

test-compiler-simple:
	g++ -std=c++17 -g gc.cpp objects.cpp environment.cpp hashpair.cpp ast.cpp token.cpp settings.cpp vm/tests/test_compiler_simple.cpp vm/compiler.cpp vm/code.cpp vm/definition.cpp vm/instructions.cpp vm/emitted_instruction.cpp vm/compilation_scope.cpp vm/opcode.cpp -o test_compiler_simple
	./test_compiler_simple

test-compile:
	g++ -std=c++17 -g gc.cpp objects.cpp environment.cpp hashpair.cpp ast.cpp token.cpp settings.cpp vm/tests/test_compiler.cpp vm/compiler.cpp vm/code.cpp vm/definition.cpp vm/instructions.cpp vm/emitted_instruction.cpp vm/compilation_scope.cpp vm/opcode.cpp -o test_compiler
	./test_compiler

test-vm:
	g++ -std=c++17 -g vm/tests/test_vm.cpp vm/vm.cpp vm/frame.cpp vm/objects.cpp vm/instructions.cpp vm/code.cpp vm/definition.cpp objects.cpp gc.cpp hashpair.cpp ast.cpp environment.cpp settings.cpp token.cpp -o test_vm
	./test_vm

clean-win:
	del token.o lexer.o ast.o parser.o objects.o evaluator.o repl.o main.o main.exe
