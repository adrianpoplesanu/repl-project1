#ifndef __AST_H
#define __AST_H

#include "token.h"
#include <string>
#include <vector>

enum StatementType {
    ST_LET_STATEMENT,
    ST_RETURN_STATEMENT,
    ST_EXPRESSION_STATEMENT
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
};

class Ad_AST_Integer : public Ad_AST_Node {
public:
    Token token;
    int value;

    Ad_AST_Integer();
    Ad_AST_Integer(Token, int);
    virtual std::string ToString();
};

class Ad_AST_Statement : public Ad_AST_Node {

};

class Ad_AST_Expression : public Ad_AST_Node {

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
    //std::string value;
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

    Ad_AST_ReturnStatement();
    Ad_AST_ReturnStatement(Token);
    std::string TokenLiteral();
    std::string ToString();
};

class Ad_AST_ExpressionStatement : public Ad_AST_Statement {
public:
    Token token;

    Ad_AST_ExpressionStatement();
    Ad_AST_ExpressionStatement(Token);
    std::string TokenLiteral();
    std::string ToString();
};

bool StatementIs(Ad_AST_Statement *stmt, StatementType st) {
    return stmt->type == st;
}

#endif