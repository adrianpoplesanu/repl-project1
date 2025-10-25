#include "token.h"

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

Token::Token() {

}

Token::Token(std::string l, TokenType tt) {
    type = tt;
    literal = l;
}

Token::~Token() {

}

std::string Token::GetLiteral() {
    return literal;
}

void Token::SetLiteral(std::string l) {
    literal = l;
}

TokenType Token::GetType() {
    return type;
}

void Token::SetType(TokenType tt) {
    type = tt;
}

std::string Token::ToString() {
    return "Token[type: " + token_type_map[type] + ", literal: " + literal + "]";
}