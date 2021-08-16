all:
	g++ -g -std=c++11 token.cpp -c -o token.o
	g++ -g -std=c++11 lexer.cpp -c -o lexer.o
	g++ -g -std=c++11 ast.cpp -c -o ast.o
	g++ -g -std=c++11 parser.cpp -c -o parser.o
	g++ -g -std=c++11 objects.cpp -c -o objects.o
	g++ -g -std=c++11 listobject.cpp -c -o listobject.o
	g++ -g -std=c++11 environment.cpp -c -o environment.o
	g++ -g -std=c++11 evaluator.cpp -c -o evaluator.o
	g++ -g -std=c++11 signal.cpp -c -o signal.o
	g++ -g -std=c++11 repl.cpp -c -o repl.o
	g++ -g -std=c++11 test_objects.cpp -c -o test_objects.o
	g++ -g -std=c++11 tests.cpp -c -o tests.o
	g++ -g -std=c++11 test_parser.cpp -c -o test_parser.o
	g++ -g -std=c++11 utils.cpp -c -o utils.o
	g++ -g -std=c++11 builtins.cpp -c -o builtins.o
	g++ -g -std=c++11 main.cpp -c -o main.o
	g++ -g -std=c++11 main.o -o main

clean:
	find . -type f ! -name "*.h" ! -name "*.cpp" ! -name "README.md" ! -name "Makefile" ! -name "*.*" -delete
	find . -name "*.o" -delete

clean-win:
	del token.o lexer.o ast.o parser.o objects.o evaluator.o repl.o main.o main.exe