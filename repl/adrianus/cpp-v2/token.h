#ifndef __TOKEN_H
#define __TOKEN_H

#include <string>
#include <map>
#include <unordered_map>

enum TokenType {
    TT_ILLEGAL,
    TT_ASSIGN,
    TT_EOF,
    TT_LET,
    TT_RETURN,
    TT_BREAK,
    TT_CONTINUE,
    TT_WHILE,
    TT_FOR,
    TT_FOREACH,
    TT_FUNC,
    TT_FUNCTION,
    TT_METHOD,
    TT_DEF,
    TT_FUN,
    TT_IDENT,
    TT_INT,
    TT_FLOAT,
    TT_PLUS,
    TT_MINUS,
    TT_SLASH,
    TT_ASTERISK,
    TT_MODULUS,
    TT_BANG,
    TT_COMMA,
    TT_SEMICOLON,
    TT_LPAREN,
    TT_RPAREN,
    TT_LBRACE,
    TT_RBRACE,
    TT_LT,
    TT_GT,
    TT_LTE,
    TT_GTE,
    TT_EQ,
    TT_NOT_EQ,
    TT_IF,
    TT_ELSE,
    TT_TRUE,
    TT_FALSE,
    TT_DOUBLE_QUOTES,
    TT_SINGLE_QUOTES,
    TT_AND,
    TT_OR,
    TT_LBRACKET,
    TT_RBRACKET,
    TT_COLON,
    TT_DOT,
    TT_CLASS,
    TT_THIS,
    TT_MULTICOMMENT,
    TT_SINGLECOMMENT,
    TT_PLUSPLUS,
    TT_MINUSMINUS,
    TT_PLUS_EQ,
    TT_MINUS_EQ,
    TT_NULL,
    TT_SUPER
};

extern std::unordered_map<TokenType, std::string> token_type_map;
extern std::unordered_map<std::string, TokenType> keywords;

class Token {
public:
    std::string literal;
    TokenType type;

    Token();
    Token(std::string, TokenType);
    ~Token();
    std::string GetLiteral();
    void SetLiteral(std::string);
    TokenType GetType();
    void SetType(TokenType);
    std::string ToString();
};

#endif