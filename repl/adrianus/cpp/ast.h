#ifndef __AST_H
#define __AST_H

#include "token.h"
#include <string>
#include <vector>

enum StatementType {
    ST_LET_STATEMENT,
    ST_RETURN_STATEMENT,
    ST_EXPRESSION_STATEMENT,
    ST_IDENTIFIER,
    ST_INTEGER,
    ST_BOOLEAN,
    ST_STATEMENT,
    ST_EXPRESSION,
    ST_INFIX_EXPRESSION,
    ST_PREFIX_EXPRESSION
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
    Ad_AST_Expression* value;

    Ad_AST_LetStatement();
    Ad_AST_LetStatement(Token);
    ~Ad_AST_LetStatement();
    std::string TokenLiteral();
    std::string ToString();
};

class Ad_AST_ReturnStatement : public Ad_AST_Statement {
public:
    Token token;
    Ad_AST_Expression* value;

    Ad_AST_ReturnStatement();
    Ad_AST_ReturnStatement(Token);
    ~Ad_AST_ReturnStatement();
    std::string TokenLiteral();
    std::string ToString();
};

class Ad_AST_ExpressionStatement : public Ad_AST_Statement {
public:
    Token token;
    Ad_AST_Expression* expression;

    Ad_AST_ExpressionStatement();
    Ad_AST_ExpressionStatement(Token);
    ~Ad_AST_ExpressionStatement();
    std::string TokenLiteral();
    std::string ToString();
};

bool StatementIs(Ad_AST_Statement *stmt, StatementType st) {
    return stmt->type == st;
}

void free_Ad_AST_Node_memory(Ad_AST_Node*);

#endif