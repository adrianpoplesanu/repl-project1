#include "token.h"

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
    return TT_ILLEGAL;
}

void Token::SetType(TokenType tt) {
    type = tt;
}

std::string Token::ToString() {
    return "Token[type: " + token_type_map[type] + ", literal: " + literal + "]";
}