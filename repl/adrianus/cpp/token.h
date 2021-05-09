#ifndef __TOKEN_H
#define __TOKEN_H

#include <string>
#include <map>

enum TokenType {
	TT_ILLEGAL,
	TT_EOF_FLAG,
	TT_IDENT,
	TT_ASSIGN,
	TT_LPAREN,
	TT_RPAREN,
	TT_LBRACE,
	TT_RBRACE,
	TT_LT,
	TT_GT,
	TT_EQ,
	TT_NOT_EQ,
	TT_IF,
	TT_ELSE,
	TT_RETURN,
	TT_SEMICOLON,
	TT_COMMA,
	TT_PLUS,
	TT_MINUS,
	TT_SLASH,
	TT_ASTERISK,
	TT_BANG,
	TT_TRUE_VALUE,
	TT_FALSE_VALUE,
	TT_FUNCTION,
	TT_LET
};

std::map<TokenType, std::string> token_type_map = {
	{TT_ASSIGN, "ASSIGN"},
	{TT_ILLEGAL, "ILLEGAL"},
	{TT_EOF_FLAG, "EOF"},
	{TT_TRUE_VALUE, "TRUE"},
	{TT_FALSE_VALUE, "FALSE"},
	{TT_IDENT, "IDENT"},
	{TT_FUNCTION, "FUNCTION"},
	{TT_LET, "LET"},
	{TT_LPAREN, "LPAREN"},
	{TT_RPAREN, "RPAREN"},
	{TT_LBRACE, "LBRACE"},
	{TT_RBRACE, "RBRACE"},
	{TT_LT, "LT"},
	{TT_GT, "GT"},
	{TT_EQ, "EQ"},
	{TT_NOT_EQ, "NOT_EQ"},
	{TT_IF, "IF"},
	{TT_ELSE, "ELSE"},
	{TT_RETURN, "RETURN"},
	{TT_SEMICOLON, "SEMICOLON"},
	{TT_COMMA, "COMMA"},
	{TT_PLUS, "PLUS"},
	{TT_MINUS, "MINUS"},
	{TT_SLASH, "SLASH"},
	{TT_ASTERISK, "ASTERISK"},
	{TT_BANG, "BANG"}
};

std::map<std::string, TokenType> keywords = {
	{"if", TT_IF},
	{"else", TT_ELSE},
	{"return", TT_RETURN},
	{"true", TT_TRUE_VALUE},
	{"false", TT_FALSE_VALUE},
	{"fn", TT_FUNCTION},
	{"let", TT_LET}
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