#include <iostream>
#include "parser.h"
#include "token.h"
#include "lexer.cpp"
#include "ast.cpp"

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

void Parser::ParseProgram(Ad_AST_Program &program) {
    int limit = 0;
    while (current_token.type != TT_EOF) {
        std::cout << current_token.ToString() << "\n";
        Ad_AST_Statement *stmt = ParseStatement();
        if (stmt) {
            std::cout << "created statement: ";
            PrintStatement(stmt);
        } else {
            std::cout << "unable to create statement\n";
        }
        if (stmt) program.statements.push_back(stmt);
        NextToken();
        ++limit;
        if (limit > 99) {
            std::cout << "exiting loop, max statement limit reached";
            break;
        }
    }
}

void Parser::PrintStatement(Ad_AST_Statement* stmt) {
    Ad_AST_LetStatement* current;
    switch(stmt->type) {
        case ST_LET_STATEMENT:
            current = (Ad_AST_LetStatement*)stmt;
            std::cout << stmt << " " << &(current->name) << " " << &(current->value) << " " << current->ToString() << "\n";
        break;
        case ST_RETURN_STATEMENT:
            std::cout << stmt << " " << ((Ad_AST_ReturnStatement*)stmt)->ToString() << "\n";
        break;
        case ST_EXPRESSION_STATEMENT:
            std::cout << stmt << " " << ((Ad_AST_ExpressionStatement*)stmt)->ToString() << "\n";
        break;
        default:
            std::cout << "Unknown Statement\n";
        break;
    }
}

void Parser::NextToken() {
    current_token = peek_token;
    peek_token = lexer.NextToken();
}

bool Parser::CurrentTokenIs(TokenType tt) {
    return current_token.type == tt;
}

bool Parser::PeekTokenIs(TokenType tt) {
    return peek_token.type == tt;
}

bool Parser::ExpectPeek(TokenType tt) {
    if (PeekTokenIs(tt)) {
        NextToken();
        return true;
    } else {
        PeekError("mesaj de eroare aici");
        return false;
    }
}

void Parser::PeekError(std::string msg) {
    errors.push_back(msg);
}

Ad_AST_Statement* Parser::ParseStatement() {
    if (current_token.type == TT_LET)
        return ParseLetStatement();
    if (current_token.type == TT_RETURN)
        return ParseReturnStatement();
    return ParseExpressionStatement();
}

Ad_AST_Statement* Parser::ParseLetStatement() {
    Ad_AST_LetStatement* stmt = new Ad_AST_LetStatement(current_token);

    if (!ExpectPeek(TT_IDENT)) {
        delete stmt;
        return NULL;
    }

    stmt->name = Identifier(current_token, current_token.literal);

    if (!ExpectPeek(TT_ASSIGN)) {
        delete stmt;
        return NULL;
    }

    NextToken();
    stmt->value = "expression not parsed yet";
    if (CurrentTokenIs(TT_SEMICOLON)) {
        NextToken();
    }
    return stmt;
}

Ad_AST_Statement* Parser::ParseReturnStatement() {
    // TODO
    return NULL;
}

Ad_AST_Statement* Parser::ParseExpressionStatement() {
    // TODO
    return NULL;
}