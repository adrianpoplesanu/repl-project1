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
        Ad_AST_Node *obj = *it;
        free_Ad_AST_Node_memory(obj);
    }
}

std::string Ad_AST_Program::TokenLiteral() {
    return "todo - implement this";
}

std::string Ad_AST_Program::ToString() {
    std::string out = "";
    for (std::vector<Ad_AST_Node*>::iterator it = statements.begin() ; it != statements.end(); ++it) {
        Ad_AST_Node *generic = *it;
        std::cout << generic->ToString() << "\n";
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
    free_Ad_AST_Node_memory(value);
}

std::string Ad_AST_LetStatement::ToString() {
    std::string out = "LetStatement [";
    out +=  token.literal + "] <" + name.value + ">: ";
    if (value) {
        //std::cout << (*value).type;
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
    type = ST_RETURN_STATEMENT;
    token = t;
}

std::string Ad_AST_ReturnStatement::ToString() {
    std::string out = "ReturnStatement [";
    out +=  token.literal + "] <" + value->ToString() + ">";
    return out;
}

Ad_AST_ExpressionStatement::Ad_AST_ExpressionStatement() {
    type = ST_EXPRESSION_STATEMENT;
}

Ad_AST_ExpressionStatement::Ad_AST_ExpressionStatement(Token t) {
    type = ST_EXPRESSION_STATEMENT;
    token = t;
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

Ad_AST_Integer::~Ad_AST_Integer() {

}

std::string Ad_AST_Integer::ToString() {
    return std::to_string(value);
}

Ad_AST_Boolean::Ad_AST_Boolean() {
    type = ST_BOOLEAN;
}

Ad_AST_Boolean::Ad_AST_Boolean(Token t, bool val) {
    type = ST_BOOLEAN;
    token = t;
    value = val;
}

Ad_AST_Boolean::~Ad_AST_Boolean() {

}

std::string Ad_AST_Boolean::ToString() {
    return std::to_string(value);
}


Ad_AST_InfixExpression::Ad_AST_InfixExpression() {
    type = ST_INFIX_EXPRESSION;
}

Ad_AST_InfixExpression::~Ad_AST_InfixExpression() {
    free_Ad_AST_Node_memory(left);
    free_Ad_AST_Node_memory(right);
}

std::string Ad_AST_InfixExpression::ToString() {
    return "(" + left->ToString() + _operator + right->ToString() + ")";
}

Ad_AST_PefixExpression::Ad_AST_PefixExpression() {
    type = ST_PREFIX_EXPRESSION;
}

Ad_AST_PefixExpression::Ad_AST_PefixExpression(Token t, std::string op) {
    type = ST_PREFIX_EXPRESSION;
    token = t;
    _operator = op;
}

Ad_AST_PefixExpression::~Ad_AST_PefixExpression() {
    free_Ad_AST_Node_memory(right);
}

std::string Ad_AST_PefixExpression::ToString() {
    std::string out;
    out = "(" + _operator + right->ToString() + ")";
    return out;
}

void free_Ad_AST_Node_memory(Ad_AST_Node* obj) {
    switch(obj->type) {
        case ST_LET_STATEMENT:
            delete (Ad_AST_LetStatement*)obj;
        break;
        case ST_RETURN_STATEMENT:
            delete (Ad_AST_ReturnStatement*)obj;
        break;
        case ST_EXPRESSION_STATEMENT:
            delete (Ad_AST_ExpressionStatement*)obj;
        break;
        case ST_IDENTIFIER:
            delete (Ad_AST_Identifier*)obj;
        break;
        case ST_INTEGER:
            delete (Ad_AST_Integer*)obj;
        break;
        case ST_BOOLEAN:
            delete (Ad_AST_Boolean*)obj;
        break;
        case ST_STATEMENT:
            delete (Ad_AST_Statement*)obj;
        break;
        case ST_EXPRESSION:
            delete (Ad_AST_Expression*)obj;
        break;
        case ST_INFIX_EXPRESSION:
            delete (Ad_AST_InfixExpression*)obj;
        break;
        case ST_PREFIX_EXPRESSION:
            delete (Ad_AST_PefixExpression*)obj;
        break;
        default:
            std::cout << "MEMORY ERROR!!!: " << obj->type << "\n";
        break;
    }
}