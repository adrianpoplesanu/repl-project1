#ifndef __PARSER_H
#define __PARSER_H

#include "lexer.h"
#include "ast.h"
#include <string>
#include <map>
#include <vector>

enum ParseType {
    PT_LOWEST,
    PT_EQUALS,
    PT_LESSGREATER,
    PT_SUM,
    PT_PRODUCT,
    PT_PREFIX,
    PT_CALL
};

std::map<TokenType, ParseType> precedences = {
    {TT_EQ, PT_EQUALS},
    {TT_NOT_EQ, PT_EQUALS},
    {TT_LT, PT_LESSGREATER},
    {TT_GT, PT_LESSGREATER},
    {TT_PLUS, PT_SUM},
    {TT_MINUS, PT_SUM},
    {TT_SLASH, PT_PRODUCT},
    {TT_ASTERISK, PT_PRODUCT},
    {TT_LPAREN, PT_CALL}
};

class Parser {
public:
    std::string source;
    Token current_token;
    Token peek_token;
    std::vector<std::string> errors;
    Lexer lexer;
    Parser();
    ~Parser();
    void Load(std::string);
    void ParseProgram(Ad_AST_Program &);
    void NextToken();
    bool CurrentTokenIs(TokenType);
    bool PeekTokenIs(TokenType);
    bool ExpectPeek(TokenType);
    void PeekError(std::string);
    Ad_AST_Statement* ParseStatement();
    Ad_AST_Statement* ParseLetStatement();
    Ad_AST_Statement* ParseReturnStatement();
    Ad_AST_Statement* ParseExpressionStatement();
    void PrintStatement(Ad_AST_Statement*);
};

#endif