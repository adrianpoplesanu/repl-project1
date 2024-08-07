#ifndef __PARSER_H
#define __PARSER_H

#include "lexer.h"
#include "ast.h"
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

enum PrecedenceType {
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

std::unordered_map<TokenType, PrecedenceType> precedences = {
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
    {TT_MODULUS, PT_PRODUCT},
    {TT_ASTERISK, PT_PRODUCT},
    {TT_LPAREN, PT_CALL},
    {TT_LBRACKET, PT_INDEX},
    {TT_DOT, PT_MEMBERACCESS},
    {TT_PLUSPLUS, PT_PLUSPLUS},
    {TT_MINUSMINUS, PT_PLUSPLUS},
    {TT_PLUS_EQ, PT_ASSIGN},
    {TT_MINUS_EQ, PT_ASSIGN}
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
    typedef Ad_AST_Node* (Parser::*StatementCallback)();
    std::unordered_map<TokenType, PrefixCallback> prefixParseFns;
    std::unordered_map<TokenType, InfixCallback> infixParseFns;
    std::unordered_map<TokenType, StatementCallback> statementParseFns;

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
    PrecedenceType PeekPrecedence();
    PrecedenceType CurPrecedence();
    Ad_AST_Node* ParseStatement();
    Ad_AST_Node* ParseLetStatement();
    Ad_AST_Node* ParseReturnStatement();
    Ad_AST_Node* ParseExpressionStatement();
    void PrintNode(Ad_AST_Node*);
    Ad_AST_Node* ParseIdentifier();
    Ad_AST_Node* ParseIntegerLiteral();
    Ad_AST_Node* ParseFloatLiteral();
    Ad_AST_Node* ParsePrefixExpression();
    Ad_AST_Node* ParseBoolean();
    Ad_AST_Node* ParseGroupedExpression();
    Ad_AST_Node* ParseIfStatement();
    Ad_AST_Node* ParseBlockStatement();
    Ad_AST_Node* ParseSingleBlockStatement();
    Ad_AST_Node* ParseFunctionLiteral();
    Ad_AST_Node* ParseExpression(PrecedenceType);
    Ad_AST_Node* ParseInfixExpression(Ad_AST_Node*);
    Ad_AST_Node* ParseCallExpression(Ad_AST_Node*);
    Ad_AST_Node* ParseWhileExpression();
    Ad_AST_Node* ParseStringLiteral();
    Ad_AST_Node* ParseListLiteral();
    Ad_AST_Node* ParseHashLiteral();
    std::pair<std::vector<Ad_AST_Node*>, std::vector<Ad_AST_Node*>> ParseCallArguments();
    std::pair<std::vector<Ad_AST_Node*>, std::vector<Ad_AST_Node*>> ParseFunctionParameters();
    std::vector<Ad_AST_Node*> ParseListExpressions();
    Ad_AST_Node* ParseIndexExpression(Ad_AST_Node*);
    Ad_AST_Node* ParseIndexExpressionOld(Ad_AST_Node*);
    Ad_AST_Node* ParseAssignExpression(Ad_AST_Node*);
    Ad_AST_Node* ParseDefStatement();
    Ad_AST_Node* ParseFunExpression();
    Ad_AST_Node* ParseFunctionExpression();
    Ad_AST_Node* ParseMultiCommentStatement();
    Ad_AST_Node* ParseSingleLineCommentStatement();
    Ad_AST_Node* ParseClassStatement();
    Ad_AST_Node* ParseMemberAccess(Ad_AST_Node*);
    Ad_AST_Node* ParsePrefixPlusPlus();
    Ad_AST_Node* ParsePostfixPlusPlus(Ad_AST_Node*);
    Ad_AST_Node* ParseForExpression();
    Ad_AST_Node* ParseBreakStatement();
    Ad_AST_Node* ParseContinueStatement();
    Ad_AST_Node* ParseNullExpression();
    Ad_AST_Node* ParseThisExpression();
    Ad_AST_Node* parseSuperExpression();
    Ad_AST_Node* parsePlusEqualsExpression(Ad_AST_Node*);
};

#endif