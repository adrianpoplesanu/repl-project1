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
    std::cout << "cleaning up\n";
     for (std::vector<Ad_AST_Node*>::iterator it = statements.begin() ; it != statements.end(); ++it) {
         Ad_AST_Node *generic = *it;
         switch(generic->type) {
             case ST_LET_STATEMENT:
                //delete generic;
                delete (Ad_AST_LetStatement*)generic; // mi se pare ca merge mai bine asa
             break;
             case ST_RETURN_STATEMENT:
                //delete generic;
                delete (Ad_AST_ReturnStatement*)generic; // mi se pare ca merge mai bine asa
             break;
             case ST_EXPRESSION_STATEMENT:
                //delete generic;
                delete (Ad_AST_ExpressionStatement*)generic; // mi se pare ca merge mai bine asa
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
        switch(generic->type) {
            case ST_LET_STATEMENT:
               std::cout << ((Ad_AST_LetStatement*)generic)->ToString();
            break;
            case ST_RETURN_STATEMENT:
               std::cout << ((Ad_AST_ReturnStatement*)generic)->ToString();
            break;
            case ST_EXPRESSION_STATEMENT:
               std::cout << ((Ad_AST_ExpressionStatement*)generic)->ToString();
            break;
            default:
            break;
        }
    }
    return out;
}

Ad_AST_LetStatement::Ad_AST_LetStatement() {

}

Ad_AST_LetStatement::Ad_AST_LetStatement(Token t) {
    token = t;
    name = Identifier();
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
    // TODO
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
    // TODO
    type = ST_EXPRESSION_STATEMENT;
}

std::string Ad_AST_ExpressionStatement::ToString() {
    std::string out = "ExpressionStatement [";
    out +=  token.literal + "]";
    return out;
}

Identifier::Identifier() {

}

Identifier::Identifier(Token t, std::string val) {
    token = t;
    value = val;
}