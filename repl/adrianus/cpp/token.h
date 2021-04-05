#ifndef __TOKEN_H
#define __TOKEN_H

#include <string>

enum TokenType {
	ILLEGAL,
	IDENT,
	LPAREN,
	RPAREN,
	LBRACE,
	RBRACE
};

class Token {
private:
	std::string literal;
	TokenType type;
public:
	Token();
	Token(std::string, TokenType);
	~Token();
	std::string GetLiteral();
	void SetLiteral(std::string);
	TokenType GetType();
	void SetType(TokenType);
};

#endif