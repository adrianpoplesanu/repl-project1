all:
	g++ -O3 -finline-functions -funroll-loops -fomit-frame-pointer -g -std=c++17 -lssl -lcrypto -I/opt/homebrew/opt/openssl@3/include/ -L/opt/homebrew/opt/openssl@3/lib/ main.cpp -c -o main.o
	g++ -O3 -finline-functions -funroll-loops -fomit-frame-pointer -g -std=c++17 -lssl -lcrypto -I/opt/homebrew/opt/openssl@3/include/ -L/opt/homebrew/opt/openssl@3/lib/ main.o -o main

clean:
	find . -type f ! -name "*.h" ! -name "*.cpp" ! -name "README.md" ! -name "Makefile" ! -name "*.*" -delete
	find . -name "*.o" -delete

clean-win:
	del token.o lexer.o ast.o parser.o objects.o evaluator.o repl.o main.o main.exe
