#include <iostream>
#include "parser.h"
#include "token.h"
#include "lexer.cpp"

Parser::Parser() {

}

Parser::~Parser() {

}

void Parser::Load(std::string s) {
    source = s;
    lexer.Load(source);
    lexer.ReadChar();
}

void Parser::BuildStatements() {
    while (!lexer.IsEOF()) {
        Token token = lexer.NextToken();
        std::cout << "<" << token_type_map.at(token.type) << "> [" << token.literal << "]\n";
    }
}