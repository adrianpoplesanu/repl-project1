#ifndef __PARSER_H
#define __PARSER_H

#include "lexer.h"
#include "ast.h"
#include <string>
#include <map>
#include <vector>
#include <functional>

//#typedef std::function<Ad_AST_Node*(Ad_AST_Node*) caca

//std::map<TokenType, std::function<void()>> prefixParseFns;
//std::map<TokenType, std::function<Ad_AST_Node*(Ad_AST_Node*)>> infixParseFns;

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

    typedef Ad_AST_Node* (Parser::*PrefixCallback)();
    typedef Ad_AST_Node* (Parser::*InfixCallback)(Ad_AST_Node*);
    std::map<TokenType, PrefixCallback> prefixParseFns;
    std::map<TokenType, InfixCallback> infixParseFns;

    Parser();
    ~Parser();
    void AddPrefixInfixFunctions();
    void TestInfixFunction(TokenType);
    void Load(std::string);
    void ParseProgram(Ad_AST_Program &);
    void NextToken();
    bool CurrentTokenIs(TokenType);
    bool PeekTokenIs(TokenType);
    bool ExpectPeek(TokenType);
    void PeekError(std::string);
    ParseType PeekPrecedence();
    Ad_AST_Statement* ParseStatement();
    Ad_AST_Statement* ParseLetStatement();
    Ad_AST_Statement* ParseReturnStatement();
    Ad_AST_Statement* ParseExpressionStatement();
    void PrintStatement(Ad_AST_Statement*);
    Ad_AST_Node* ParseIdentifier();
    Ad_AST_Node* ParseIntegerLiteral();
    Ad_AST_Node* ParsePrefixExpression();
    Ad_AST_Node* ParseBoolean();
    Ad_AST_Node* ParseGroupedExpression();
    Ad_AST_Node* ParseIfExpression();
    Ad_AST_Node* ParseFunctionLiteral();
    Ad_AST_Expression* ParseExpression(ParseType);
    Ad_AST_Node* ParseInfixExpression(Ad_AST_Node*);
    Ad_AST_Node* ParseCallExpression(Ad_AST_Node*);
};

#endif