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
    NextToken();
    NextToken();
}

void Parser::ParseProgram() {
    int limit = 0;
    while (current_token.type != TT_EOF) {
        std::cout << current_token.ToString() << "\n";
        NextToken();
        ++limit;
        if (limit > 99) {
            std::cout << "exiting loop, limit reached";
            break;
        }
    }
}

void Parser::NextToken() {
    current_token = peek_token;
    peek_token = lexer.NextToken();
}