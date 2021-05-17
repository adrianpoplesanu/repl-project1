#include <iostream>
#include "parser.h"
#include "token.h"
#include "lexer.cpp"
#include "ast.cpp"

Parser::Parser() {
    prefixParseFns.insert(std::make_pair(TT_IDENT, &Parser::ParseIdentifier));
    prefixParseFns.insert(std::make_pair(TT_INT, &Parser::ParseIntegerLiteral));
    prefixParseFns.insert(std::make_pair(TT_BANG, &Parser::ParsePrefixExpression));
    prefixParseFns.insert(std::make_pair(TT_MINUS, &Parser::ParsePrefixExpression));
    prefixParseFns.insert(std::make_pair(TT_TRUE, &Parser::ParseBoolean));
    prefixParseFns.insert(std::make_pair(TT_FALSE, &Parser::ParseBoolean));
    prefixParseFns.insert(std::make_pair(TT_LPAREN, &Parser::ParseGroupedExpression));
    prefixParseFns.insert(std::make_pair(TT_IF, &Parser::ParseIfExpression));
    prefixParseFns.insert(std::make_pair(TT_FUNCTION, &Parser::ParseFunctionLiteral));
    infixParseFns.insert(std::make_pair(TT_PLUS, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_MINUS, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_SLASH, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_ASTERISK, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_EQ, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_NOT_EQ, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_LT, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_GT, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_LPAREN, &Parser::ParseCallExpression));
}

Parser::~Parser() {

}

void Parser::AddPrefixInfixFunctions() {

}

void Parser::TestInfixFunction(TokenType tt) {
    InfixCallback fp = infixParseFns[tt]; // fp - function pointer
    Ad_AST_Node* res = (this->*fp)(NULL);
    delete res;
}

void Parser::Load(std::string s) {
    source = s;
    lexer.Load(source);
    NextToken();
    NextToken();
}

void Parser::ParseProgram(Ad_AST_Program &program) {
    int limit = 0;
    while (current_token.type != TT_EOF) {
        std::cout << current_token.ToString() << "\n";
        Ad_AST_Statement *stmt = ParseStatement();
        if (stmt) program.statements.push_back(stmt);
        NextToken();
        ++limit;
        if (limit > 99) {
            std::cout << "exiting loop, max statement limit reached";
            break;
        }
    }
}

void Parser::PrintStatement(Ad_AST_Statement* stmt) {
    Ad_AST_LetStatement* current;
    switch(stmt->type) { // nu cred ca mai am nevoie de switch pentru ca ToString() e acum virtuala
        case ST_LET_STATEMENT:
            current = (Ad_AST_LetStatement*)stmt;
            std::cout << stmt << " " << &(current->name) << " " << &(current->value) << " " << current->ToString() << "\n";
        break;
        case ST_RETURN_STATEMENT:
            std::cout << stmt << " " << ((Ad_AST_ReturnStatement*)stmt)->ToString() << "\n";
        break;
        case ST_EXPRESSION_STATEMENT:
            std::cout << stmt << " " << ((Ad_AST_ExpressionStatement*)stmt)->ToString() << "\n";
        break;
        default:
            std::cout << "Unknown Statement\n";
        break;
    }
}

void Parser::NextToken() {
    current_token = peek_token;
    peek_token = lexer.NextToken();
}

bool Parser::CurrentTokenIs(TokenType tt) {
    return current_token.type == tt;
}

bool Parser::PeekTokenIs(TokenType tt) {
    return peek_token.type == tt;
}

bool Parser::ExpectPeek(TokenType tt) {
    if (PeekTokenIs(tt)) {
        NextToken();
        return true;
    } else {
        PeekError("ERROR: mesaj de eroare aici");
        return false;
    }
}

ParseType Parser::PeekPrecedence() {
    if (precedences.find(peek_token.type) != precedences.end()) {
        return precedences[peek_token.type];
    }
    return PT_LOWEST;
}

ParseType Parser::CurPrecedence() {
    if (precedences.find(current_token.type) != precedences.end()) {
        return precedences[current_token.type];
    }
    return PT_LOWEST;
}

void Parser::PeekError(std::string msg) {
    errors.push_back(msg);
}

Ad_AST_Statement* Parser::ParseStatement() {
    if (current_token.type == TT_LET)
        return ParseLetStatement();
    if (current_token.type == TT_RETURN)
        return ParseReturnStatement();
    return ParseExpressionStatement();
}

Ad_AST_Statement* Parser::ParseLetStatement() {
    Ad_AST_LetStatement* stmt = new Ad_AST_LetStatement(current_token);

    if (!ExpectPeek(TT_IDENT)) {
        delete stmt;
        return NULL;
    }

    stmt->name = Ad_AST_Identifier(current_token, current_token.literal);

    if (!ExpectPeek(TT_ASSIGN)) {
        delete stmt;
        return NULL;
    }

    NextToken();
    stmt->value = ParseExpression(PT_LOWEST);
    if (CurrentTokenIs(TT_SEMICOLON)) {
        NextToken();
    }
    //std::cout << "aici2\n";
    return stmt;
}

Ad_AST_Statement* Parser::ParseReturnStatement() {
    // TODO
    Ad_AST_ReturnStatement* stmt = new Ad_AST_ReturnStatement();
    return stmt;
}

Ad_AST_Statement* Parser::ParseExpressionStatement() {
    // TODO
    //Ad_AST_ExpressionStatement* stmt = new Ad_AST_ExpressionStatement();
    return NULL;
}

Ad_AST_Node* Parser::ParseIdentifier() {
    std::cout << "parse identifier\n";
    Ad_AST_Identifier *identifier = new Ad_AST_Identifier(current_token, current_token.literal);
    return identifier;
}

Ad_AST_Node* Parser::ParseInfixExpression(Ad_AST_Node* left) {
    Ad_AST_InfixExpression* expr = new Ad_AST_InfixExpression();
    expr->token = current_token;
    expr->_operator = current_token.literal;
    expr->left = left;
    ParseType preced = CurPrecedence();
    NextToken();
    expr->right = ParseExpression(preced);
    return expr;
}

Ad_AST_Node* Parser::ParseCallExpression(Ad_AST_Node* node) {
    // TODO
    return NULL;
}

Ad_AST_Node* Parser::ParseIntegerLiteral() {
    Ad_AST_Integer* integer_node = new Ad_AST_Integer();
    int val = stoi(current_token.literal);
    integer_node->token = current_token;
    integer_node->value = val;
    return integer_node;
}

Ad_AST_Node* Parser::ParsePrefixExpression() {
    // TODO
    return NULL;
}

Ad_AST_Node* Parser::ParseBoolean() {
    // TODO
    return NULL;
}

Ad_AST_Node* Parser::ParseGroupedExpression() {
    // TODO
    return NULL;
}

Ad_AST_Node* Parser::ParseIfExpression() {
    // TODO
    return NULL;
}

Ad_AST_Node* Parser::ParseFunctionLiteral() {
    // TODO
    return NULL;
}

Ad_AST_Expression* Parser::ParseExpression(ParseType precedence) {
    if (prefixParseFns.find(current_token.type) == prefixParseFns.end()) {
        std::cout << token_type_map[current_token.type] << " ";
        std::cout << "oops! 1\n";
        return NULL;
    }
    PrefixCallback prefix = prefixParseFns[current_token.type];

    Ad_AST_Expression* leftExp = (Ad_AST_Expression*)(this->*prefix)();

    while(!PeekTokenIs(TT_SEMICOLON) && (precedence < PeekPrecedence())) {
        if (infixParseFns.find(peek_token.type) == infixParseFns.end()) {
            std::cout << "oops! 2\n";
            return leftExp;
        }
        InfixCallback infix = infixParseFns[peek_token.type];
        NextToken();
        leftExp = (Ad_AST_Expression*)(this->*infix)(leftExp);
    }
    return leftExp;
}