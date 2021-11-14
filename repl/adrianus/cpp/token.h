#ifndef __TOKEN_H
#define __TOKEN_H

#include <string>
#include <map>

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
    TT_FUNCTION,
    TT_DEF,
    TT_IDENT,
    TT_INT,
    TT_PLUS,
    TT_MINUS,
    TT_SLASH,
    TT_ASTERISK,
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
    TT_STRING,
    TT_AND,
    TT_OR,
    TT_LBRACKET,
    TT_RBRACKET,
    TT_COLON,
    TT_DOT,
    TT_CLASS,
    TT_THIS,
    TT_STARTCOMMENT,
    TT_ENDCOMMENT,
    TT_PLUSPLUS
};

std::map<TokenType, std::string> token_type_map = {
    {TT_ILLEGAL, "ILLEGAL"},
    {TT_ASSIGN, "ASSIGN"},
    {TT_EOF, "EOF"},
    {TT_LET, "LET"},
    {TT_RETURN, "RETURN"},
    {TT_BREAK, "BREAK"},
    {TT_CONTINUE, "CONTINUE"},
    {TT_WHILE, "WHILE"},
    {TT_FOR, "FOR"},
    {TT_FOREACH, "FOREACH"},
    {TT_FUNCTION, "FUNCTION"},
    {TT_DEF, "DEF"},
    {TT_IDENT, "IDENT"},
    {TT_INT, "INT"},
    {TT_PLUS, "PLUS"},
    {TT_MINUS, "MINUS"},
    {TT_SLASH, "SLASH"},
    {TT_ASTERISK, "ASTERISK"},
    {TT_BANG, "BANG"},
    {TT_COMMA, "COMMA"},
    {TT_SEMICOLON, "SEMICOLON"},
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
    {TT_TRUE, "TRUE"},
    {TT_FALSE, "FALSE"},
    {TT_STRING, "STRING"},
    {TT_AND, "AND"},
    {TT_OR, "OR"},
    {TT_LBRACKET, "LBRACKET"},
    {TT_RBRACKET, "RBRACKET"},
    {TT_COLON, "COLON"},
    {TT_DOT, "DOT"},
    {TT_CLASS, "CLASS"},
    {TT_THIS, "THIS"},
    {TT_STARTCOMMENT, "STARTCOMMENT"},
    {TT_ENDCOMMENT, "ENDCOMMENT"},
    {TT_PLUSPLUS, "INCREMENT_OPERATOR"}
};

std::map<std::string, TokenType> keywords = {
    {"let", TT_LET},
    {"return", TT_RETURN},
    {"break", TT_BREAK},
    {"continue", TT_CONTINUE},
    {"while", TT_WHILE},
    {"for", TT_FOR},
    {"function", TT_FUNCTION},
    {"def", TT_DEF},
    {"if", TT_IF},
    {"else", TT_ELSE},
    {"true", TT_TRUE},
    {"false", TT_FALSE},
    {"and", TT_AND},
    {"or", TT_OR},
    {"class", TT_CLASS},
    {"this", TT_THIS}
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