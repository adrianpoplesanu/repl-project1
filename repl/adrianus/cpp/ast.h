#ifndef __AST_H
#define __AST_H

#include "token.h"
#include <string>
#include <vector>

enum StatementType {
    ST_PROGRAM,
    ST_LET_STATEMENT,
    ST_RETURN_STATEMENT,
    ST_EXPRESSION_STATEMENT,
    ST_IDENTIFIER,
    ST_INTEGER,
    ST_BOOLEAN,
    ST_STATEMENT,
    ST_EXPRESSION,
    ST_INFIX_EXPRESSION,
    ST_PREFIX_EXPRESSION,
    ST_CALL_EXPRESSION,
    ST_IF_EXPRESSION,
    ST_BLOCK_STATEMENT,
    ST_FUNCTION_LITERAL,
    ST_WHILE_EXPRESSION
};

std::map<StatementType, std::string> statement_type_map = {
    {ST_PROGRAM, "Program"},
    {ST_LET_STATEMENT, "LetStatement"},
    {ST_RETURN_STATEMENT, "ReturnStatement"},
    {ST_EXPRESSION_STATEMENT, "ExpressionStatement"},
    {ST_IDENTIFIER, "Identifier"},
    {ST_INTEGER, "Integer"},
    {ST_BOOLEAN, "Boolean"},
    {ST_STATEMENT, "Statement"},
    {ST_EXPRESSION, "Expression"},
    {ST_INFIX_EXPRESSION, "InfixExpression"},
    {ST_PREFIX_EXPRESSION, "PrefixExpression"},
    {ST_CALL_EXPRESSION, "CallExpression"},
    {ST_IF_EXPRESSION, "IfExpression"},
    {ST_BLOCK_STATEMENT, "BlockStatement"},
    {ST_FUNCTION_LITERAL, "FunctionLiteral"},
    {ST_WHILE_EXPRESSION, "WhileExpression"}
};

class Ad_AST_Node {
public:
    StatementType type;
    std::string TokenLiteral();
    virtual std::string ToString();
};

class Ad_AST_Identifier : public Ad_AST_Node {
public:
    Token token;
    std::string value;

    Ad_AST_Identifier();
    Ad_AST_Identifier(Token, std::string);
    virtual std::string ToString();
};

class Ad_AST_Integer : public Ad_AST_Node {
public:
    Token token;
    int value;

    Ad_AST_Integer();
    Ad_AST_Integer(Token, int);
    ~Ad_AST_Integer();
    virtual std::string ToString();
};

class Ad_AST_Boolean : public Ad_AST_Node {
public:
    Token token;
    bool value;

    Ad_AST_Boolean();
    Ad_AST_Boolean(Token, bool);
    ~Ad_AST_Boolean();
    virtual std::string ToString();
};

class Ad_AST_Statement : public Ad_AST_Node {

};

class Ad_AST_Expression : public Ad_AST_Node {

};

class Ad_AST_InfixExpression : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node *left;
    std::string _operator;
    Ad_AST_Node *right;

    Ad_AST_InfixExpression();
    ~Ad_AST_InfixExpression();
    virtual std::string ToString();
};

class Ad_AST_PefixExpression : public Ad_AST_Node {
public:
    Token token;
    std::string _operator;
    Ad_AST_Node *right;

    Ad_AST_PefixExpression();
    Ad_AST_PefixExpression(Token, std::string);
    ~Ad_AST_PefixExpression();
    virtual std::string ToString();
};

class Ad_AST_IfExpression : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* condition;
    Ad_AST_Node* consequence;
    Ad_AST_Node* alternative;

    Ad_AST_IfExpression();
    Ad_AST_IfExpression(Token);
    ~Ad_AST_IfExpression();
    virtual std::string ToString();
};

class Ad_AST_BlockStatement : public Ad_AST_Node {
public:
    Token token;
    std::vector<Ad_AST_Node*> statements;

    Ad_AST_BlockStatement();
    Ad_AST_BlockStatement(Token);
    ~Ad_AST_BlockStatement();
    virtual std::string ToString();
};

class Ad_AST_CallExpression : public Ad_AST_Node {
public:
    Token token;
    std::vector<Ad_AST_Node*> arguments;
    Ad_AST_Node* function;

    Ad_AST_CallExpression();
    Ad_AST_CallExpression(Token);
    Ad_AST_CallExpression(Token, Ad_AST_Node*);
    ~Ad_AST_CallExpression();
    virtual std::string ToString();
};

class Ad_AST_FunctionLiteral : public Ad_AST_Node {
public:
    Token token;
    std::vector<Ad_AST_Node*> parameters;
    Ad_AST_Node* body;

    Ad_AST_FunctionLiteral();
    Ad_AST_FunctionLiteral(Token);
    ~Ad_AST_FunctionLiteral();
    virtual std::string ToString();
};

class Ad_AST_Program : public Ad_AST_Node {
public:
    std::vector<Ad_AST_Node*> statements;

    Ad_AST_Program();
    ~Ad_AST_Program();
    std::string TokenLiteral();
    virtual std::string ToString();
};

class Ad_AST_LetStatement : public Ad_AST_Statement {
public:
    Token token;
    Ad_AST_Identifier name;
    Ad_AST_Node* value;

    Ad_AST_LetStatement();
    Ad_AST_LetStatement(Token);
    ~Ad_AST_LetStatement();
    std::string TokenLiteral();
    std::string ToString();
};

class Ad_AST_ReturnStatement : public Ad_AST_Statement {
public:
    Token token;
    Ad_AST_Node* value;

    Ad_AST_ReturnStatement();
    Ad_AST_ReturnStatement(Token);
    ~Ad_AST_ReturnStatement();
    std::string TokenLiteral();
    std::string ToString();
};

class Ad_AST_ExpressionStatement : public Ad_AST_Statement {
public:
    Token token; // this variable is never instantiated, maybe i don't need to print it in the depth first search
    Ad_AST_Node* expression;

    Ad_AST_ExpressionStatement();
    Ad_AST_ExpressionStatement(Token);
    ~Ad_AST_ExpressionStatement();
    std::string TokenLiteral();
    std::string ToString();
};

class Ad_AST_WhileExpression : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* condition;
    Ad_AST_Node* consequence; // Ad_AST_BlockStatement

    Ad_AST_WhileExpression();
    Ad_AST_WhileExpression(Token);
    ~Ad_AST_WhileExpression();
    std::string TokenLiteral();
    std::string ToString();
};

bool StatementIs(Ad_AST_Statement *stmt, StatementType st) {
    return stmt->type == st;
}

void free_Ad_AST_Node_memory(Ad_AST_Node*);

#endif