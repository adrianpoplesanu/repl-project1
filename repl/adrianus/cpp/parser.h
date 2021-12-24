#ifndef __PARSER_H
#define __PARSER_H

#include "lexer.h"
#include "ast.h"
#include <string>
#include <map>
#include <vector>

enum ParseType {
    PT_LOWEST,
    PT_ASSIGN,
    PT_EQUALS,
    PT_CONDITIONAL,
    PT_LESSGREATER,
    PT_SUM,
    PT_PRODUCT,
    PT_PLUSPLUS,
    PT_PREFIX,
    PT_CALL,
    PT_INDEX,
    PT_MEMBERACCESS
};

std::map<TokenType, ParseType> precedences = {
    {TT_ASSIGN, PT_ASSIGN},
    {TT_EQ, PT_EQUALS},
    {TT_NOT_EQ, PT_EQUALS},
    {TT_LT, PT_LESSGREATER},
    {TT_GT, PT_LESSGREATER},
    {TT_LTE, PT_LESSGREATER},
    {TT_GTE, PT_LESSGREATER},
    {TT_AND, PT_CONDITIONAL},
    {TT_OR, PT_CONDITIONAL},
    {TT_PLUS, PT_SUM},
    {TT_MINUS, PT_SUM},
    {TT_SLASH, PT_PRODUCT},
    {TT_ASTERISK, PT_PRODUCT},
    {TT_LPAREN, PT_CALL},
    {TT_LBRACKET, PT_INDEX},
    {TT_DOT, PT_MEMBERACCESS},
    {TT_PLUSPLUS, PT_PLUSPLUS}
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
    ParseType CurPrecedence();
    Ad_AST_Node* ParseStatement();
    Ad_AST_Node* ParseLetStatement();
    Ad_AST_Node* ParseReturnStatement();
    Ad_AST_Node* ParseExpressionStatement();
    void PrintNode(Ad_AST_Node*);
    Ad_AST_Node* ParseIdentifier();
    Ad_AST_Node* ParseIntegerLiteral();
    Ad_AST_Node* ParsePrefixExpression();
    Ad_AST_Node* ParseBoolean();
    Ad_AST_Node* ParseGroupedExpression();
    Ad_AST_Node* ParseIfExpression();
    Ad_AST_Node* ParseBlockStatement();
    Ad_AST_Node* ParseFunctionLiteral();
    Ad_AST_Node* ParseExpression(ParseType);
    Ad_AST_Node* ParseInfixExpression(Ad_AST_Node*);
    Ad_AST_Node* ParseCallExpression(Ad_AST_Node*);
    Ad_AST_Node* ParseWhileExpression();
    Ad_AST_Node* ParseStringLiteral();
    Ad_AST_Node* ParseListLiteral();
    Ad_AST_Node* ParseHashLiteral();
    std::vector<Ad_AST_Node*> ParseCallArguments();
    std::vector<Ad_AST_Node*> ParseFunctionParameters();
    std::vector<Ad_AST_Node*> ParseListExpressions();
    Ad_AST_Node* ParseIndexExpression(Ad_AST_Node*);
    Ad_AST_Node* ParseAssignExpression(Ad_AST_Node*);
    Ad_AST_Node* ParseDefExpression();
    Ad_AST_Node* ParseComment();
    Ad_AST_Node* ParseClassStatement();
    Ad_AST_Node* ParseMemberAccess(Ad_AST_Node*);
    Ad_AST_Node* ParsePrefixPlusPlus();
    Ad_AST_Node* ParsePostfixPlusPlus(Ad_AST_Node*);
    Ad_AST_Node* ParseForExpression();
    Ad_AST_Node* ParseNullExpression();
    Ad_AST_Node* ParseThisExpression();
};

#endif