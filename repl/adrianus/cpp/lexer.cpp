#include "lexer.h"
#include "token.cpp"
#include <iostream>

Lexer::Lexer() {

}

Lexer::~Lexer() {

}

void Lexer::Load(std::string s) {
    source = s;
    position = 0;
    readPosition = 0;
    ReadChar();
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
    while(IsLetter() || IsDigit()) {
        ReadChar();
    }
    return source.substr(start, position - start);
}

std::string Lexer::ReadNumber() {
    int start = position;
    while(IsDigit()) {
        ReadChar();
    }
    if (IsFloatDot()) {
        ReadChar();
        while(IsDigit()) {
            ReadChar();
        }
    }
    return source.substr(start, position - start);
}

TokenType Lexer::LookupIdent(std::string ident) {
    if (keywords.find(ident) != keywords.end()) {
        return keywords.at(ident);
    }
    return TT_IDENT;
}

std::string Lexer::ReadDoubleQuotesString() {
    ReadChar();
    int start = position;
    while (current_char != '"') {
        if (current_char == '\\' && PeekChar() == '"') {  // escaping \"
            ReadChar();
        }
        ReadChar();
    }
    return source.substr(start, position - start);
}

std::string Lexer::ReadSingleQuotesString() {
    ReadChar();
    int start = position;
    while (current_char != '\'') {
        if (current_char == '\\' && PeekChar() == '\'') {  // escaping \'
            ReadChar();
        }
        ReadChar();
    }
    return source.substr(start, position - start);
}

char Lexer::PeekChar() {
    return source[readPosition];
}

bool Lexer::IsEOF() {
    return current_char == 0;
}

bool Lexer::IsFloatDot() {
    return current_char == '.';
}

Token Lexer::NextToken() {
    Token token;
    SkipWhitespace();
    bool do_read_char_after_lexing = true;
    switch(current_char) {
        case 0:
            token.type = TT_EOF;
            token.literal = "";
        break;
        case '+':
            if (PeekChar() == '+') {
                ReadChar();
                token.type = TT_PLUSPLUS;
                token.literal = "++";
            } else {
                token.type = TT_PLUS;
                token.literal = current_char;
            }
        break;
        case '-':
            token.type = TT_MINUS;
            token.literal = current_char;
        break;
        case '*':
            token.type = TT_ASTERISK;
            token.literal = current_char;
        break;
        case '%':
            token.type = TT_MODULUS;
            token.literal = current_char;
        break;
        case '/':
            if (PeekChar() == '*') {
                ReadChar();
                token.type = TT_MULTICOMMENT;
                token.literal = "/*";
                while (!(current_char == '*' && PeekChar() == '/')) {
                    ReadChar();
                }
                ReadChar();
                ReadChar();
            } else if (PeekChar() == '/') {
                token.type = TT_SINGLECOMMENT;
                token.literal = "//";
                ReadChar();
                while (current_char != '\n' && current_char != 0) {
                    ReadChar();
                }
            } else {
                token.type = TT_SLASH;
                token.literal = current_char;
            }
        break;
        case '<':
            if (PeekChar() == '=') {
                ReadChar();
                token.type = TT_LTE;
                token.literal = "<=";
            } else {
                token.type = TT_LT;
                token.literal = current_char;
            }
        break;
        case '>':
            if (PeekChar() == '=') {
                ReadChar();
                token.type = TT_GTE;
                token.literal = ">=";
            } else {
                token.type = TT_GT;
                token.literal = current_char;
            }
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
        case ':':
            token.type = TT_COLON;
            token.literal = current_char;
        break;
        case '"':
            token.type = TT_DOUBLE_QUOTES;
            token.literal = ReadDoubleQuotesString();
        break;
        case '\'':
            token.type = TT_SINGLE_QUOTES;
            token.literal = ReadSingleQuotesString();
        break;
        case '[':
            token.type = TT_LBRACKET;
            token.literal = current_char;
        break;
        case ']':
            token.type = TT_RBRACKET;
            token.literal = current_char;
        break;
        case '.':
            token.type = TT_DOT;
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
                do_read_char_after_lexing = false;
            } else if (IsDigit()) {
                std::string num = ReadNumber();
                if (num.find(".") != std::string::npos) {
                    token.type = TT_FLOAT;
                } else {
                    token.type = TT_INT;
                }
                token.literal = num;
                do_read_char_after_lexing = false;
            } else {
                token.type = TT_ILLEGAL;
                token.literal = "";
            }
        break;
    }
    if (do_read_char_after_lexing) ReadChar();
    return token;
}