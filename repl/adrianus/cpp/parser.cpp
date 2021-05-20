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
        //std::cout << current_token.ToString() << "\n";
        Ad_AST_Statement *stmt = (Ad_AST_Statement*)ParseStatement();
        if (stmt) program.statements.push_back(stmt);
        //if (stmt) PrintNode(stmt);
        NextToken();
        ++limit;
        if (limit > 99) {
            std::cout << "exiting loop, max statement limit reached";
            break;
        }
    }
}

void Parser::PrintNode(Ad_AST_Node* node) {
    std::cout << node->ToString() << "\n";
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

Ad_AST_Node* Parser::ParseStatement() {
    if (current_token.type == TT_LET)
        return ParseLetStatement();
    if (current_token.type == TT_RETURN)
        return ParseReturnStatement();
    return ParseExpressionStatement();
}

Ad_AST_Node* Parser::ParseLetStatement() {
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
    return stmt;
}

Ad_AST_Node* Parser::ParseReturnStatement() {
    Ad_AST_ReturnStatement* stmt = new Ad_AST_ReturnStatement(current_token);
    NextToken();
    stmt->value = ParseExpression(PT_LOWEST);
    while (!CurrentTokenIs(TT_SEMICOLON) && !CurrentTokenIs(TT_EOF)) {
        NextToken();
    }
    return stmt;
}

Ad_AST_Node* Parser::ParseExpressionStatement() {
    Ad_AST_ExpressionStatement* stmt = new Ad_AST_ExpressionStatement();
    stmt->expression = ParseExpression(PT_LOWEST);
    if (PeekTokenIs(TT_SEMICOLON)) {
        NextToken();
    }
    return stmt;
}

Ad_AST_Node* Parser::ParseIdentifier() {
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

Ad_AST_Node* Parser::ParseCallExpression(Ad_AST_Node* function) {
    Ad_AST_CallExpression* expr = new Ad_AST_CallExpression(current_token, function);
    expr->arguments = ParseCallArguments();
    return expr;
}

std::vector<Ad_AST_Node*> Parser::ParseCallArguments() {
    std::vector<Ad_AST_Node*> args;
    if (PeekTokenIs(TT_RPAREN)) {
        NextToken();
        return args;
    }
    NextToken();
    args.push_back(ParseExpression(PT_LOWEST));
    while (PeekTokenIs(TT_COMMA)) {
        NextToken();
        NextToken();
        args.push_back(ParseExpression(PT_LOWEST));
    }
    if (!ExpectPeek(TT_RPAREN)) {
        std::vector<Ad_AST_Node*> empty; // i don't like this, it should be NULL
        return empty;
    }
    return args;
}

Ad_AST_Node* Parser::ParseIntegerLiteral() {
    Ad_AST_Integer* integer_node = new Ad_AST_Integer();
    int val = stoi(current_token.literal);
    integer_node->token = current_token;
    integer_node->value = val;
    return integer_node;
}

Ad_AST_Node* Parser::ParsePrefixExpression() {
    Ad_AST_PefixExpression* expr = new Ad_AST_PefixExpression(current_token, current_token.literal);
    NextToken();
    expr->right = ParseExpression(PT_PREFIX);
    return expr;
}

Ad_AST_Node* Parser::ParseBoolean() {
    Ad_AST_Boolean* boolean_object = new Ad_AST_Boolean(current_token, CurrentTokenIs(TT_TRUE));
    return boolean_object;
}

Ad_AST_Node* Parser::ParseGroupedExpression() {
    NextToken();
    Ad_AST_Node* expr = ParseExpression(PT_LOWEST);
    if (!ExpectPeek(TT_RPAREN)) {
        return NULL;
    }
    return expr;
}

Ad_AST_Node* Parser::ParseIfExpression() {
    Ad_AST_IfExpression* expr = new Ad_AST_IfExpression(current_token);
    if (!ExpectPeek(TT_LPAREN)) {
        return NULL;
    }
    NextToken();
    expr->condition = ParseExpression(PT_LOWEST);
    if (!ExpectPeek(TT_RPAREN)) {
        return NULL;
    }
    if (!ExpectPeek(TT_LBRACE)) {
        return NULL;
    }
    expr->consequence = ParseBlockStatement();

    if (PeekTokenIs(TT_ELSE)) {
        NextToken();
        if (!ExpectPeek(TT_LBRACE)) {
            return NULL;
        }
        expr->alternative = ParseBlockStatement();
    }
    return expr;
}

Ad_AST_Node* Parser::ParseFunctionLiteral() {
    Ad_AST_FunctionLiteral* fun_lit = new Ad_AST_FunctionLiteral(current_token);
    if (!ExpectPeek(TT_LPAREN)) {
        return NULL;
    }
    fun_lit->parameters = ParseFunctionParameters();
    if (!ExpectPeek(TT_LBRACE)) {
        return NULL;
    }
    fun_lit->body = ParseBlockStatement();
    return fun_lit;
}

std::vector<Ad_AST_Node*> Parser::ParseFunctionParameters() {
    std::vector<Ad_AST_Node*> identifiers;
    if (PeekTokenIs(TT_RPAREN)) {
        NextToken();
        return identifiers;
    }
    NextToken();
    Ad_AST_Identifier* ident = new Ad_AST_Identifier(current_token, current_token.literal);
    identifiers.push_back(ident);
    while (PeekTokenIs(TT_COMMA)) {
        NextToken();
        NextToken();
        ident = new Ad_AST_Identifier(current_token, current_token.literal);
        identifiers.push_back(ident);
    }
    if (!ExpectPeek(TT_RPAREN)) {
        std::vector<Ad_AST_Node*> empty; // i don't like this, it should be NULL
        return empty;
    }
    return identifiers;
}

Ad_AST_Node* Parser::ParseBlockStatement() {
    Ad_AST_BlockStatement* block = new Ad_AST_BlockStatement(current_token);
    NextToken();
    while(!CurrentTokenIs(TT_RBRACE) && !CurrentTokenIs(TT_EOF)) {
        Ad_AST_Statement* stmt = (Ad_AST_Statement*)ParseStatement();
        if (stmt) {
            block->statements.push_back(stmt);
        }
        NextToken();
    }
    return block;
}

Ad_AST_Node* Parser::ParseExpression(ParseType precedence) {
    if (prefixParseFns.find(current_token.type) == prefixParseFns.end()) {
        return NULL;
    }
    PrefixCallback prefix = prefixParseFns[current_token.type];

    Ad_AST_Node* leftExp = (this->*prefix)();

    while(!PeekTokenIs(TT_SEMICOLON) && (precedence < PeekPrecedence())) {
        if (infixParseFns.find(peek_token.type) == infixParseFns.end()) {
            return leftExp;
        }
        InfixCallback infix = infixParseFns[peek_token.type];
        NextToken();
        leftExp = (this->*infix)(leftExp);
    }
    return leftExp;
}