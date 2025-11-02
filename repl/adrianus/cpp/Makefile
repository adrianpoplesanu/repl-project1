all:
	g++ -O3 -finline-functions -funroll-loops -fomit-frame-pointer -g -std=c++17 -lssl -lcrypto -I/opt/homebrew/opt/openssl@3/include/ -L/opt/homebrew/opt/openssl@3/lib/ main.cpp -c -o main.o
	g++ -O3 -finline-functions -funroll-loops -fomit-frame-pointer -g -std=c++17 -lssl -lcrypto -I/opt/homebrew/opt/openssl@3/include/ -L/opt/homebrew/opt/openssl@3/lib/ main.o -o main

clean:
	find . -type f ! -name "*.h" ! -name "*.cpp" ! -name "README.md" ! -name "Makefile" ! -name "*.*" -delete
	find . -name "*.o" -delete

test-code:
	g++ -std=c++17 -g vm/tests/test_code.cpp vm/code.cpp vm/definition.cpp vm/instructions.cpp -o test_code
	./test_code

test-compiler:
	g++ -std=c++17 -g gc.cpp objects.cpp environment.cpp hashpair.cpp ast.cpp token.cpp settings.cpp vm/tests/test_compiler_simple.cpp vm/compiler.cpp vm/code.cpp vm/definition.cpp vm/instructions.cpp vm/emitted_instruction.cpp vm/compilation_scope.cpp -o test_compiler_simple
#	g++ -std=c++17 -g vm/tests/test_compiler_simple.cpp -o test_compiler_simple
#	g++ -std=c++17 -g vm/tests/test_compiler_simple.cpp vm/compiler.cpp vm/code.cpp vm/definition.cpp vm/instructions.cpp vm/emitted_instruction.cpp vm/compilation_scope.cpp -o test_compiler_simple
	./test_compiler_simple

clean-win:
	del token.o lexer.o ast.o parser.o objects.o evaluator.o repl.o main.o main.exe
