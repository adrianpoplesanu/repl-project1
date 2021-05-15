#include "ast.h"

std::string Ad_AST_Node::TokenLiteral() {
    return "node literal - implement in subclass";
}

std::string Ad_AST_Node::ToString() {
    return "node string repr - implement in subclass";
}

std::string Ad_AST_Program::TokenLiteral() {
    return "todo - implement this";
}

std::string Ad_AST_Program::ToString() {
    return "todo - implement this";
}

Ad_AST_LetStatement::Ad_AST_LetStatement() {

}

Ad_AST_LetStatement::Ad_AST_LetStatement(Token t) {
    token = t;
    name = "";
    value = "";
    type = ST_LET_STATEMENT;
}

std::string Ad_AST_LetStatement::ToString() {
    std::string out = "LetStatement [";
    out +=  token.literal + "]";
    return out;
}

Ad_AST_ReturnStatement::Ad_AST_ReturnStatement() {

}

Ad_AST_ReturnStatement::Ad_AST_ReturnStatement(Token t) {
    token = t;
    //name = "";
    //value = "";
    type = ST_RETURN_STATEMENT;
}

std::string Ad_AST_ReturnStatement::ToString() {
    std::string out = "ReturnStatement [";
    out +=  token.literal + "]";
    return out;
}

Ad_AST_ExpressionStatement::Ad_AST_ExpressionStatement() {

}

Ad_AST_ExpressionStatement::Ad_AST_ExpressionStatement(Token t) {
    token = t;
    //name = "";
    //value = "";
    type = ST_EXPRESSION_STATEMENT;
}

std::string Ad_AST_ExpressionStatement::ToString() {
    std::string out = "ExpressionStatement [";
    out +=  token.literal + "]";
    return out;
}