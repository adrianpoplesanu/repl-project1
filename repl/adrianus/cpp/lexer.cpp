#include "lexer.h"

Lexer::Lexer() {

}

Lexer::~Lexer() {

}

void Lexer::Load(std::string s) {
    source = s;
}

void Lexer::ReadChar() {
    if (readPosition < source.length()) {
        current_char = source[readPosition];
    } else {
        current_char = 0;
    }
    position = readPosition;
    ++readPosition;
}

void Lexer::SkipWhitespace() {
    while (current_char == ' ' || current_char == '\t' || current_char == '\r' || current_char == '\n') {
        ReadChar();
    }
}

bool Lexer::IsLetter() {
    return (('a' <= current_char) && (current_char <= 'z')) || (('A' <= current_char) && (current_char <= 'Z')) || current_char == '_';
}

bool Lexer::IsDigit() {
    return ('0' <= current_char) && (current_char <= '9');
}

std::string Lexer::ReadIdentifier() {
    // TODO
    return "";
}

std::string Lexer::ReadNumber() {
    // TODO
    return "";
}

TokenType Lexer::LookupIdent(std::string ident) {
    // TODO
    return TT_ILLEGAL;
}

char Lexer::PeekChar() {
    // TODO
    return 0;
}

Token Lexer::NextToken() {
    Token token;
    SkipWhitespace();
    switch(current_char) {
        case 0:
            token.type = TT_EOF;
            token.literal = "";
        break;
        case '+':
            token.type = TT_PLUS;
            token.literal = current_char;
        break;
        case '-':
            token.type = TT_MINUS;
            token.literal = current_char;
        break;
        case '*':
            token.type = TT_ASTERISK;
            token.literal = current_char;
        break;
        case '/':
            token.type = TT_SLASH;
            token.literal = current_char;
        break;
        case '<':
            token.type = TT_LT;
            token.literal = current_char;
        break;
        case '>':
            token.type = TT_GT;
            token.literal = current_char;
        break;
        case ',':
            token.type = TT_COMMA;
            token.literal = current_char;
        break;
        case ';':
            token.type = TT_SEMICOLON;
            token.literal = current_char;
        break;
        case '(':
            token.type = TT_LPAREN;
            token.literal = current_char;
        break;
        case ')':
            token.type = TT_RPAREN;
            token.literal = current_char;
        break;
        case '{':
            token.type = TT_LBRACE;
            token.literal = current_char;
        break;
        case '}':
            token.type = TT_RBRACE;
            token.literal = current_char;
        break;
        case '=':
        break;
        case '!':
        break;
        default:
            if (IsLetter()) {

            }
            if (IsDigit()) {

            }
        break;
    }
    ReadChar();
    return token;
}