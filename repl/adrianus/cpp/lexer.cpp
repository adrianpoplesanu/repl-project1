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
    int start = position;
    while(IsLetter()) {
        ReadChar();
    }
    return source.substr(start, position - start);
}

std::string Lexer::ReadNumber() {
    int start = position;
    while(IsDigit()) {
        ReadChar();
    }
    return source.substr(start, position - start);
}

TokenType Lexer::LookupIdent(std::string ident) {
    if (keywords.find(ident) != keywords.end()) {
        return keywords.at(ident);
    }
    return TT_IDENT;
}

char Lexer::PeekChar() {
    return source[readPosition];
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
            if (PeekChar() == '=') {
                ReadChar();
                token.type = TT_EQ;
                token.literal = "==";
            } else {
                token.type = TT_ASSIGN;
                token.literal = current_char;
            }
        break;
        case '!':
            if (PeekChar() == '=') {
                ReadChar();
                token.type = TT_NOT_EQ;
                token.literal = "!=";
            } else {
                token.type = TT_BANG;
                token.literal = current_char;
            }
        break;
        default:
            if (IsLetter()) {
                std::string ident = ReadIdentifier();
                token.type = LookupIdent(ident);
                token.literal = ident;
            } else if (IsDigit()) {
                std::string num = ReadNumber();
                token.type = TT_INT;
                token.literal = num;
            } else {
                token.type = TT_ILLEGAL;
                token.literal = "";
            }
        break;
    }
    if (token.type != TT_IDENT && token.type != TT_INT) ReadChar();
    return token;
}