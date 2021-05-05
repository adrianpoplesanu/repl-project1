#ifndef __LEXER_H
#define __LEXER_H

#include <string>

class Lexer {
private:
	std::string source;
public:
	Lexer();
	~Lexer();
	void Load(std::string);
};

#endif