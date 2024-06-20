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

std::unordered_map<TokenType, std::string> token_type_map = {
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
    {TT_FUNC, "FUNC"},
    {TT_FUNCTION, "FUNCTION"},
    {TT_METHOD, "METHOD"},
    {TT_DEF, "DEF"},
    {TT_FUN, "FUN"},
    {TT_IDENT, "IDENT"},
    {TT_INT, "INT"},
    {TT_FLOAT, "FLOAT"},
    {TT_PLUS, "PLUS"},
    {TT_MINUS, "MINUS"},
    {TT_SLASH, "SLASH"},
    {TT_ASTERISK, "ASTERISK"},
    {TT_MODULUS, "MODULUS"},
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
    {TT_DOUBLE_QUOTES, "DOUBLE QUOTES"},
    {TT_SINGLE_QUOTES, "SINGLE QUOTES"},
    {TT_AND, "AND"},
    {TT_OR, "OR"},
    {TT_LBRACKET, "LBRACKET"},
    {TT_RBRACKET, "RBRACKET"},
    {TT_COLON, "COLON"},
    {TT_DOT, "DOT"},
    {TT_CLASS, "CLASS"},
    {TT_THIS, "THIS"},
    {TT_MULTICOMMENT, "MULTICOMMENT"},
    {TT_SINGLECOMMENT, "SINGLECOMMENT"},
    {TT_PLUSPLUS, "INCREMENT_OPERATOR"},
    {TT_MINUSMINUS, "DECREMENT_OPERATOR"},
    {TT_PLUS_EQ, "PLUS_EQUAL"},
    {TT_MINUS_EQ, "MINUS_EQUAL"},
    {TT_NULL, "NULL"},
    {TT_SUPER, "SUPER"}
};

std::unordered_map<std::string, TokenType> keywords = {
    {"let", TT_LET},
    {"return", TT_RETURN},
    {"break", TT_BREAK},
    {"continue", TT_CONTINUE},
    {"while", TT_WHILE},
    {"for", TT_FOR},
    {"func", TT_FUNC},
    {"function", TT_FUNCTION},
    {"method", TT_METHOD},
    {"def", TT_DEF},
    {"fun", TT_FUN},
    {"if", TT_IF},
    {"else", TT_ELSE},
    {"true", TT_TRUE},
    {"false", TT_FALSE},
    {"and", TT_AND},
    {"or", TT_OR},
    {"class", TT_CLASS},
    {"this", TT_THIS},
    {"null", TT_NULL},
    {"super", TT_SUPER}
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