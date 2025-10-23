#ifndef __AD_AST_H
#define __AD_AST_H

#include "token.h"
#include "settings.h"
#include <string>
#include <vector>

enum StatementType {
    ST_PROGRAM,
    ST_LET_STATEMENT,
    ST_RETURN_STATEMENT,
    ST_EXPRESSION_STATEMENT,
    ST_IDENTIFIER,
    ST_INTEGER,
    ST_FLOAT,
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
    ST_INDEX_EXPRESSION,
    ST_HASH_LITERAL,
    ST_ASSIGN_STATEMENT,
    ST_DEF_STATEMENT,
    ST_CLASS_STATEMENT,
    ST_MEMBER_ACCESS,
    ST_COMMENT,
    ST_PREFIX_INCREMENT,
    ST_POSTFIX_INCREMENT,
    ST_FOR_EXPRESSION,
    ST_BREAK_STATEMENT,
    ST_CONTINUE_STATEMENT,
    ST_NULL_EXPRESSION,
    ST_THIS_EXPRESSION,
    ST_SUPER_EXPRESSION,
    ST_PLUS_EQUALS,
    ST_MINUS_EQUALS
};

extern std::unordered_map<StatementType, std::string> statement_type_map;

class Ad_AST_Node {
public:
    StatementType type;
    int ref_count;
    virtual std::string TokenLiteral();
    virtual std::string ToString();
    virtual Ad_AST_Node* copy();
};

class Ad_AST_Program : public Ad_AST_Node {
public:
    std::vector<Ad_AST_Node*> statements;

    Ad_AST_Program();
    ~Ad_AST_Program();
    std::string TokenLiteral() override;
    std::string ToString() override;
    void reset();
};

class Ad_AST_Identifier : public Ad_AST_Node {
public:
    Token token;
    std::string value;

    Ad_AST_Identifier();
    Ad_AST_Identifier(Token, std::string);
    std::string TokenLiteral() override;
    std::string ToString() override;
    Ad_AST_Node* copy() override;
};

class Ad_AST_LetStatement : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Identifier name;
    Ad_AST_Node* value;

    Ad_AST_LetStatement();
    Ad_AST_LetStatement(Token token);
    ~Ad_AST_LetStatement();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_AssignStatement : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* name;
    Ad_AST_Node* value;

    Ad_AST_AssignStatement();
    Ad_AST_AssignStatement(Token);
    ~Ad_AST_AssignStatement();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_ReturnStatement : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* value;

    Ad_AST_ReturnStatement();
    Ad_AST_ReturnStatement(Token);
    ~Ad_AST_ReturnStatement();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_ExpressionStatement : public Ad_AST_Node {
public:
    Token token; // this variable is never instantiated, maybe i don't need to print it in the depth first search
    Ad_AST_Node* expression;

    Ad_AST_ExpressionStatement();
    Ad_AST_ExpressionStatement(Token);
    ~Ad_AST_ExpressionStatement();
    std::string TokenLiteral() override;
    std::string ToString() override;
    Ad_AST_Node* copy() override;
};

class Ad_AST_Integer : public Ad_AST_Node {
public:
    Token token;
    int value;

    Ad_AST_Integer();
    Ad_AST_Integer(Token, int);
    ~Ad_AST_Integer();
    std::string ToString() override;
};

class Ad_AST_Float : public Ad_AST_Node {
public:
    Token token;
    float value;

    Ad_AST_Float();
    Ad_AST_Float(Token, float);
    ~Ad_AST_Float();
    std::string ToString() override;
};

class Ad_AST_Boolean : public Ad_AST_Node {
public:
    Token token;
    bool value;

    Ad_AST_Boolean();
    Ad_AST_Boolean(Token, bool);
    ~Ad_AST_Boolean();
    std::string ToString() override;
};

class Ad_AST_InfixExpression : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node *left;
    std::string _operator;
    Ad_AST_Node *right;

    Ad_AST_InfixExpression();
    ~Ad_AST_InfixExpression();
    std::string ToString() override;
};

class Ad_AST_PrefixExpression : public Ad_AST_Node {
public:
    Token token;
    std::string _operator;
    Ad_AST_Node *right;

    Ad_AST_PrefixExpression();
    Ad_AST_PrefixExpression(Token, std::string);
    ~Ad_AST_PrefixExpression();
    std::string ToString() override;
};

class Ad_AST_CallExpression : public Ad_AST_Node {
public:
    Token token;
    std::vector<Ad_AST_Node*> arguments;
    std::vector<Ad_AST_Node*> kw_args;
    Ad_AST_Node* function;

    Ad_AST_CallExpression();
    Ad_AST_CallExpression(Token);
    Ad_AST_CallExpression(Token, Ad_AST_Node*);
    ~Ad_AST_CallExpression();
    std::string ToString() override;
    Ad_AST_Node* copy() override;
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
    std::string ToString() override;
};

class Ad_AST_BlockStatement : public Ad_AST_Node {
public:
    Token token;
    std::vector<Ad_AST_Node*> statements;

    Ad_AST_BlockStatement();
    Ad_AST_BlockStatement(Token);
    ~Ad_AST_BlockStatement();
    std::string ToString() override;
    Ad_AST_Node* copy() override;
};

class Ad_AST_FunctionLiteral : public Ad_AST_Node {
public:
    Token token;
    std::vector<Ad_AST_Node*> parameters;
    std::vector<Ad_AST_Node*> default_params;
    Ad_AST_Node* body;

    Ad_AST_FunctionLiteral();
    Ad_AST_FunctionLiteral(Token);
    ~Ad_AST_FunctionLiteral();
    std::string ToString() override;
};

class Ad_AST_WhileExpression : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* condition;
    Ad_AST_Node* consequence; // Ad_AST_BlockStatement, maybe rename this

    Ad_AST_WhileExpression();
    Ad_AST_WhileExpression(Token);
    ~Ad_AST_WhileExpression();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_String : public Ad_AST_Node {
public:
    Token token;
    std::string value;

    Ad_AST_String();
    Ad_AST_String(Token);
    ~Ad_AST_String();
    std::string TokenLiteral() override;
    std::string ToString() override;
    Ad_AST_Node* copy() override;
};

class Ad_AST_ListLiteral : public Ad_AST_Node {
public:
    Token token;
    std::vector<Ad_AST_Node*> elements;

    Ad_AST_ListLiteral();
    Ad_AST_ListLiteral(Token);
    ~Ad_AST_ListLiteral();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_IndexExpression : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node *left;
    Ad_AST_Node *index;
    Ad_AST_Node *indexEnd;
    Ad_AST_Node *step;

    Ad_AST_IndexExpression();
    Ad_AST_IndexExpression(Token);
    Ad_AST_IndexExpression(Token, Ad_AST_Node*);
    ~Ad_AST_IndexExpression();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_HashLiteral : public Ad_AST_Node {
public:
    Token token;
    std::unordered_map<Ad_AST_Node*, Ad_AST_Node*> pairs;

    Ad_AST_HashLiteral();
    Ad_AST_HashLiteral(Token);
    Ad_AST_HashLiteral(Token, std::unordered_map<Ad_AST_Node*, Ad_AST_Node*>);
    ~Ad_AST_HashLiteral();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_Def_Statement : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* name;
    std::vector<Ad_AST_Node*> parameters;
    std::vector<Ad_AST_Node*> default_params;
    Ad_AST_Node* body;

    Ad_AST_Def_Statement();
    Ad_AST_Def_Statement(Token);
    ~Ad_AST_Def_Statement();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_Comment : public Ad_AST_Node {
public:
    Token token;

    Ad_AST_Comment();
    Ad_AST_Comment(Token);
    ~Ad_AST_Comment();
    std::string TokenLiteral() override;
    std::string ToString() override;
    Ad_AST_Node* copy() override;
};

class Ad_AST_Class : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* name;
    std::vector<Ad_AST_Node*> methods;
    std::vector<Ad_AST_Node*> attributes;
    std::vector<Ad_AST_Node*> inheritFrom;

    Ad_AST_Class();
    Ad_AST_Class(Token);
    ~Ad_AST_Class();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_MemberAccess : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* owner;
    Ad_AST_Node* member;
    std::vector<Ad_AST_Node*> arguments;
    std::vector<Ad_AST_Node*> kw_args;
    bool is_method;

    Ad_AST_MemberAccess();
    Ad_AST_MemberAccess(Token);
    Ad_AST_MemberAccess(Token, Ad_AST_Node*, Ad_AST_Node*, std::vector<Ad_AST_Node*>);
    Ad_AST_MemberAccess(Token, Ad_AST_Node*, Ad_AST_Node*, std::vector<Ad_AST_Node*>, std::vector<Ad_AST_Node*>);
    ~Ad_AST_MemberAccess();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_PrefixIncrement : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* name;
    std::string _operator;

    Ad_AST_PrefixIncrement();
    Ad_AST_PrefixIncrement(Token);
    Ad_AST_PrefixIncrement(Token, Ad_AST_Node*);
    ~Ad_AST_PrefixIncrement();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_PostfixIncrement : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* name;
    std::string _operator;

    Ad_AST_PostfixIncrement();
    Ad_AST_PostfixIncrement(Token);
    Ad_AST_PostfixIncrement(Token, Ad_AST_Node*);
    ~Ad_AST_PostfixIncrement();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_ForExprssion : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node* initialization;
    Ad_AST_Node* condition;
    Ad_AST_Node* step;
    Ad_AST_Node* body;

    Ad_AST_ForExprssion();
    Ad_AST_ForExprssion(Token);
    Ad_AST_ForExprssion(Token, Ad_AST_Node*, Ad_AST_Node*, Ad_AST_Node*, Ad_AST_Node*);
    ~Ad_AST_ForExprssion();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_BreakStatement : public Ad_AST_Node {
public:
    Token token;

    Ad_AST_BreakStatement();
    Ad_AST_BreakStatement(Token);
    ~Ad_AST_BreakStatement();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_ContinueStatement : public Ad_AST_Node {
public:
    Token token;

    Ad_AST_ContinueStatement();
    Ad_AST_ContinueStatement(Token);
    ~Ad_AST_ContinueStatement();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_Null_Expression : public Ad_AST_Node {
public:
    Token token;

    Ad_AST_Null_Expression();
    Ad_AST_Null_Expression(Token);
    ~Ad_AST_Null_Expression();
};

class Ad_AST_This_Expression : public Ad_AST_Node {
public:
    Token token;

    Ad_AST_This_Expression();
    Ad_AST_This_Expression(Token);
    ~Ad_AST_This_Expression();
};

class Ad_AST_Super_Expression : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node *target;

    Ad_AST_Super_Expression();
    Ad_AST_Super_Expression(Token);
    ~Ad_AST_Super_Expression();
};

class Ad_AST_Plus_Equals_Statement : public Ad_AST_Node {
public:
    Token token;
    Ad_AST_Node *name;
    Ad_AST_Node *value;

    Ad_AST_Plus_Equals_Statement();
    Ad_AST_Plus_Equals_Statement(Token);
    Ad_AST_Plus_Equals_Statement(Token, Ad_AST_Node*, Ad_AST_Node*);
    ~Ad_AST_Plus_Equals_Statement();
    std::string TokenLiteral() override;
    std::string ToString() override;
};

class Ad_AST_Minus_Equals_Statement : public Ad_AST_Node {
public:
    Token token;
};


void Ad_INCREF(Ad_AST_Node*);
void Ad_DECREF(Ad_AST_Node*);

void free_Ad_AST_Node_memory(Ad_AST_Node*);

#endif