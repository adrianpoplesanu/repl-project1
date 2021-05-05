#ifndef __TOKEN_H
#define __TOKEN_H

#include <string>
#include <map>

enum TokenType {
	ILLEGAL,
	EOF_FLAG,
	IDENT,
	ASSIGN,
	LPAREN,
	RPAREN,
	LBRACE,
	RBRACE,
	LT,
	GT,
	EQ,
	NOT_EQ,
	IF,
	ELSE,
	RETURN,
	SEMICOLON,
	COMMA,
	PLUS,
	MINUS,
	SLASH,
	ASTERISK,
	BANG,
	TRUE_VALUE,
	FALSE_VALUE
};

std::map<TokenType, std::string> token_type_map = {
	{ASSIGN, "ASSIGN"},
	{ILLEGAL, "ILLEGAL"}
};

std::map<std::string, TokenType> keywords = {
	{"if", IF},
	{"else", ELSE},
	{"return", RETURN},
	{"true", TRUE_VALUE},
	{"false", FALSE_VALUE}
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