all:
	g++ -g -std=c++17 token.cpp -c -o token.o
	g++ -g -std=c++17 lexer.cpp -c -o lexer.o
	g++ -g -std=c++17 ast.cpp -c -o ast.o
	g++ -g -std=c++17 parser.cpp -c -o parser.o
	g++ -g -std=c++17 hashpair.cpp -c -o hashpair.o
	g++ -g -std=c++17 objects.cpp -c -o objects.o
	g++ -g -std=c++17 listobject.cpp -c -o listobject.o
	g++ -g -std=c++17 environment_builtins.cpp -c -o environment_builtins.o
	g++ -g -std=c++17 environment.cpp -c -o environment.o
	g++ -g -std=c++17 evaluator.cpp -c -o evaluator.o
	g++ -g -std=c++17 signal.cpp -c -o signal.o
	g++ -g -std=c++17 bootstrap.cpp -c -o bootstrap.o
	g++ -g -std=c++17 repl.cpp -c -o repl.o
	g++ -g -std=c++17 test_objects.cpp -c -o test_objects.o
	g++ -g -std=c++17 tests.cpp -c -o tests.o
	g++ -g -std=c++17 test_parser.cpp -c -o test_parser.o
	g++ -g -std=c++17 test_listobject.cpp -c -o test_listobject.o
	g++ -g -std=c++17 utils.cpp -c -o utils.o
	g++ -g -std=c++17 builtins.cpp -c -o builtins.o
	g++ -g -std=c++17 main.cpp -c -o main.o
	g++ -g -std=c++17 main.o -o main

clean:
	find . -type f ! -name "*.h" ! -name "*.cpp" ! -name "README.md" ! -name "Makefile" ! -name "*.*" -delete
	find . -name "*.o" -delete

clean-win:
	del token.o lexer.o ast.o parser.o objects.o evaluator.o repl.o main.o main.exe
