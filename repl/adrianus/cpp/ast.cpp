#include "ast.h"
#include <iostream>

std::string Ad_AST_Node::TokenLiteral() {
    return "node literal - implement in subclass";
}

std::string Ad_AST_Node::ToString() {
    return "node string repr - implement in subclass";
}

Ad_AST_Program::Ad_AST_Program() {

}

Ad_AST_Program::~Ad_AST_Program() {
    for (std::vector<Ad_AST_Node*>::iterator it = statements.begin() ; it != statements.end(); ++it) {
        Ad_AST_Node *generic = *it;
        switch(generic->type) {
            case ST_LET_STATEMENT:
                delete (Ad_AST_LetStatement*)generic;
            break;
            case ST_RETURN_STATEMENT:
                delete (Ad_AST_ReturnStatement*)generic;
            break;
            case ST_EXPRESSION_STATEMENT:
                delete (Ad_AST_ExpressionStatement*)generic;
            break;
            default:
            break;
        }
    }
}

std::string Ad_AST_Program::TokenLiteral() {
    return "todo - implement this";
}

std::string Ad_AST_Program::ToString() {
    std::string out = "";
    for (std::vector<Ad_AST_Node*>::iterator it = statements.begin() ; it != statements.end(); ++it) {
        Ad_AST_Node *generic = *it;
        switch(generic->type) { // pentru ca ToString e virtual, nici nu mai am in teorie nevoie de switchul asta
            case ST_LET_STATEMENT:
                std::cout << generic->ToString(); // pentru ca le-am facut virtuale
                std::cout << "\n";
            break;
            case ST_RETURN_STATEMENT:
                std::cout << generic->ToString();
                std::cout << "\n";
            break;
            case ST_EXPRESSION_STATEMENT:
                std::cout << generic->ToString();
                std::cout << "\n";
            break;
            default:
            break;
        }
    }
    return out;
}

Ad_AST_LetStatement::Ad_AST_LetStatement() {
    type = ST_LET_STATEMENT;
}

Ad_AST_LetStatement::Ad_AST_LetStatement(Token t) {
    token = t;
    name = Ad_AST_Identifier();
    value = NULL;
    type = ST_LET_STATEMENT;
}

Ad_AST_LetStatement::~Ad_AST_LetStatement() {
    delete value;
}

std::string Ad_AST_LetStatement::ToString() {
    std::string out = "LetStatement [";
    out +=  token.literal + "] <" + name.value + ">: ";
    if (value) {
        std::cout << (*value).type;
        out += (*value).ToString();
    } else {
        out += "null expression in let";
    }
    return out;
}

Ad_AST_ReturnStatement::Ad_AST_ReturnStatement() {
    type = ST_RETURN_STATEMENT;
}

Ad_AST_ReturnStatement::Ad_AST_ReturnStatement(Token t) {
    token = t;
    // TODO
    type = ST_RETURN_STATEMENT;
}

std::string Ad_AST_ReturnStatement::ToString() {
    std::string out = "ReturnStatement [";
    out +=  token.literal + "]";
    return out;
}

Ad_AST_ExpressionStatement::Ad_AST_ExpressionStatement() {
    type = ST_EXPRESSION_STATEMENT;
}

Ad_AST_ExpressionStatement::Ad_AST_ExpressionStatement(Token t) {
    token = t;
    // TODO
    type = ST_EXPRESSION_STATEMENT;
}

std::string Ad_AST_ExpressionStatement::ToString() {
    std::string out = "ExpressionStatement [";
    out +=  token.literal + "]";
    return out;
}

Ad_AST_Identifier::Ad_AST_Identifier() {
    type = ST_IDENTIFIER;
}

Ad_AST_Identifier::Ad_AST_Identifier(Token t, std::string val) {
    type = ST_IDENTIFIER;
    token = t;
    value = val;
}

std::string Ad_AST_Identifier::ToString() {
    return value;
}

Ad_AST_Integer::Ad_AST_Integer() {
    type = ST_INTEGER;
}

Ad_AST_Integer::Ad_AST_Integer(Token t, int val) {
    type = ST_INTEGER;
    token = t;
    value = val;
}

std::string Ad_AST_Integer::ToString() {
    return std::to_string(value);
}

Ad_AST_InfixExpression::Ad_AST_InfixExpression() {
    type = ST_INFIX_EXPRESSION;
}

std::string Ad_AST_InfixExpression::ToString() {
    return "(" + left->ToString() + _operator + right->ToString() + ")";
}