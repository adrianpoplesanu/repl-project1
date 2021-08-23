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
    ST_INFIX_EXPRESSION,
    ST_PREFIX_EXPRESSION,
    ST_CALL_EXPRESSION,
    ST_IF_EXPRESSION,
    ST_BLOCK_STATEMENT,
    ST_FUNCTION_LITERAL,
    ST_WHILE_EXPRESSION,
    ST_STRING_LITERAL,
    ST_LIST_LITERAL,
    ST_INDEX_EXPRESSION
};

std::map<StatementType, std::string> statement_type_map = {
    {ST_PROGRAM, "Program"},
    {ST_LET_STATEMENT, "LetStatement"},
    {ST_RETURN_STATEMENT, "ReturnStatement"},
    {ST_EXPRESSION_STATEMENT, "ExpressionStatement"},
    {ST_IDENTIFIER, "Identifier"},
    {ST_INTEGER, "Integer"},
    {ST_BOOLEAN, "Boolean"},
    {ST_INFIX_EXPRESSION, "InfixExpression"},
    {ST_PREFIX_EXPRESSION, "PrefixExpression"},
    {ST_CALL_EXPRESSION, "CallExpression"},
    {ST_IF_EXPRESSION, "IfExpression"},
    {ST_BLOCK_STATEMENT, "BlockStatement"},
    {ST_FUNCTION_LITERAL, "FunctionLiteral"},
    {ST_WHILE_EXPRESSION, "WhileExpression"},
    {ST_STRING_LITERAL, "StringLiteral"},
    {ST_LIST_LITERAL, "ListLiteral"},
    {ST_INDEX_EXPRESSION, "IndexExpression"}
};

class Ad_AST_Node {
public:
    StatementType type;
    int ref_count;
    virtual std::string TokenLiteral();
    virtual std::string ToString();
};

class Ad_AST_Program : public Ad_AST_Node {
public:
    std::vector<Ad_AST_Node*> statements;

    Ad_AST_Program();
    ~Ad_AST_Program();
    virtual std::string TokenLiteral();
    virtual std::string ToString();
};

class Ad_AST_Identifier : public Ad_AST_Node {
public:
    Token token;
    std::string value;

    Ad_AST_Identifier();
    Ad_AST_Identifier(Token, std::string);
    virtual std::string TokenLiteral();
    virtual std::string ToString();
};

class Ad_AST_LetStatement : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Identifier name;
    Ad_AST_Node* value;

    Ad_AST_LetStatement();
    Ad_AST_LetStatement(Token);
    ~Ad_AST_LetStatement();
    virtual std::string TokenLiteral();
    virtual std::string ToString();
};

class Ad_AST_ReturnStatement : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* value;

    Ad_AST_ReturnStatement();
    Ad_AST_ReturnStatement(Token);
    ~Ad_AST_ReturnStatement();
    virtual std::string TokenLiteral();
    virtual std::string ToString();
};

class Ad_AST_ExpressionStatement : public Ad_AST_Node {
public:
    Token token; // this variable is never instantiated, maybe i don't need to print it in the depth first search
    Ad_AST_Node* expression;

    Ad_AST_ExpressionStatement();
    Ad_AST_ExpressionStatement(Token);
    ~Ad_AST_ExpressionStatement();
    virtual std::string TokenLiteral();
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

class Ad_AST_PrefixExpression : public Ad_AST_Node {
public:
    Token token;
    std::string _operator;
    Ad_AST_Node *right;

    Ad_AST_PrefixExpression();
    Ad_AST_PrefixExpression(Token, std::string);
    ~Ad_AST_PrefixExpression();
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

class Ad_AST_WhileExpression : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* condition;
    Ad_AST_Node* consequence; // Ad_AST_BlockStatement, maybe rename this

    Ad_AST_WhileExpression();
    Ad_AST_WhileExpression(Token);
    ~Ad_AST_WhileExpression();
    virtual std::string TokenLiteral();
    virtual std::string ToString();
};

class Ad_AST_String : public Ad_AST_Node {
public:
    Token token;
    std::string value;

    Ad_AST_String();
    Ad_AST_String(Token);
    ~Ad_AST_String();
    virtual std::string TokenLiteral();
    virtual std::string ToString();
};

class Ad_AST_ListLiteral : public Ad_AST_Node {
public:
    Token token;
    std::vector<Ad_AST_Node*> elements;

    Ad_AST_ListLiteral();
    Ad_AST_ListLiteral(Token);
    ~Ad_AST_ListLiteral();
    virtual std::string TokenLiteral();
    virtual std::string ToString();
};

class Ad_AST_IndexExpression : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node *left;
    int index;

    Ad_AST_IndexExpression();
    Ad_AST_IndexExpression(Token);
    ~Ad_AST_IndexExpression();
    virtual std::string TokenLiteral();
    virtual std::string ToString();
};

bool StatementIs(Ad_AST_Node *stmt, StatementType st) {
    return stmt->type == st;
}

void Ad_INCREF(Ad_AST_Node*);
void Ad_DECREF(Ad_AST_Node*);

void free_Ad_AST_Node_memory(Ad_AST_Node*);

#endif