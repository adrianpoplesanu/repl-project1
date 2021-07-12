#ifndef __LEXER_H
#define __LEXER_H

#include "token.h"
#include <string>

class Lexer {
private:
	std::string source;
	int position;
	int readPosition;
	char current_char;
public:
	Lexer();
	~Lexer();
	void Load(std::string);
	void ReadChar();
	void SkipWhitespace();
	bool IsLetter();
	bool IsDigit();
	std::string ReadIdentifier();
	std::string ReadNumber();
	TokenType LookupIdent(std::string); // (std::string ident)
	std::string ReadString();
	char PeekChar();
	Token NextToken();
	bool IsEOF();
};

#endif