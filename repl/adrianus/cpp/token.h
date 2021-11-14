#ifndef __TOKEN_H
#define __TOKEN_H

#include <string>
#include <map>

enum TokenType {
    TT_ILLEGAL,
    TT_EOF,
    TT_IDENT,
    TT_ASSIGN,
    TT_DEF,
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
    TT_RETURN,
    TT_SEMICOLON,
    TT_COMMA,
    TT_PLUS,
    TT_MINUS,
    TT_SLASH,
    TT_ASTERISK,
    TT_BANG,
    TT_TRUE,
    TT_FALSE,
    TT_FUNCTION,
    TT_LET,
    TT_INT,
    TT_WHILE,
    TT_LBRACKET,
    TT_RBRACKET,
    TT_STRING,
    TT_AND,
    TT_OR,
    TT_COLON,
    TT_DOT,
    TT_CLASS,
    TT_THIS,
    TT_STARTCOMMENT,
    TT_ENDCOMMENT,
    TT_PLUSPLUS,
    TT_CONTINUE,
    TT_BREAK
};

std::map<TokenType, std::string> token_type_map = {
    {TT_ASSIGN, "ASSIGN"},
    {TT_DEF, "DEF"},
    {TT_ILLEGAL, "ILLEGAL"},
    {TT_EOF, "EOF"},
    {TT_TRUE, "TRUE"},
    {TT_FALSE, "FALSE"},
    {TT_IDENT, "IDENT"},
    {TT_FUNCTION, "FUNCTION"},
    {TT_LET, "LET"},
    {TT_LPAREN, "LPAREN"},
    {TT_RPAREN, "RPAREN"},
    {TT_LBRACE, "LBRACE"},
    {TT_RBRACE, "RBRACE"},
    {TT_LT, "LT"},
    {TT_GT, "GT"},
    {TT_LTE, "LTE"},
    {TT_GTE, "GTE"},
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
    {TT_BANG, "BANG"},
    {TT_INT, "INT"},
    {TT_WHILE, "WHILE"},
    {TT_LBRACKET, "LBRACKET"},
    {TT_RBRACKET, "RBRACKET"},
    {TT_STRING, "STRING"},
    {TT_AND, "AND"},
    {TT_OR, "OR"},
    {TT_COLON, "COLON"},
    {TT_DOT, "DOT"},
    {TT_CLASS, "CLASS"},
    {TT_THIS, "THIS"},
    {TT_STARTCOMMENT, "STARTCOMMENT"},
    {TT_ENDCOMMENT, "ENDCOMMENT"},
    {TT_PLUSPLUS, "INCREMENT_OPERATOR"},
    {TT_CONTINUE, "CONTINUE"},
    {TT_BREAK, "BREAK"}
};

std::map<std::string, TokenType> keywords = {
    {"if", TT_IF},
    {"else", TT_ELSE},
    {"return", TT_RETURN},
    {"true", TT_TRUE},
    {"false", TT_FALSE},
    {"function", TT_FUNCTION},
    {"def", TT_DEF},
    {"let", TT_LET},
    {"while", TT_WHILE},
    {"and", TT_AND},
    {"or", TT_OR},
    {"class", TT_CLASS},
    {"this", TT_THIS},
    {"continue", TT_CONTINUE},
    {"break", TT_BREAK}
};

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