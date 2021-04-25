all:
	g++ token.cpp -c -o token.o
	g++ lexer.cpp -c -o lexer.o
	g++ ast.cpp -c -o ast.o
	g++ parser.cpp -c -o parser.o
	g++ evaluator.cpp -c -o evaluator.o
	g++ repl.cpp -c -o repl.o
	g++ main.cpp -c -o main.o
	g++ main.o -o main

clean:
	#rm main main.o token.o
	find . -type f ! -name "*.h" ! -name "*.cpp" ! -name "README.md" ! -name "Makefile" ! -name "*.*" -delete
	find . -name "*.o" -delete