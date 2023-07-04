#include <iostream>
#include "parser.h"
#include "token.h"
#include "lexer.cpp" // TODO: remove .cpp imports and use only .h and have all .cpp imports in main.cpp
//#include "lexer.h" // ^^^
#include "ast.cpp"

Parser::Parser() {
    prefixParseFns.insert(std::make_pair(TT_IDENT, &Parser::ParseIdentifier));
    prefixParseFns.insert(std::make_pair(TT_INT, &Parser::ParseIntegerLiteral));
    prefixParseFns.insert(std::make_pair(TT_FLOAT, &Parser::ParseFloatLiteral));
    prefixParseFns.insert(std::make_pair(TT_BANG, &Parser::ParsePrefixExpression));
    prefixParseFns.insert(std::make_pair(TT_MINUS, &Parser::ParsePrefixExpression));
    prefixParseFns.insert(std::make_pair(TT_TRUE, &Parser::ParseBoolean));
    prefixParseFns.insert(std::make_pair(TT_FALSE, &Parser::ParseBoolean));
    prefixParseFns.insert(std::make_pair(TT_LPAREN, &Parser::ParseGroupedExpression));
    prefixParseFns.insert(std::make_pair(TT_IF, &Parser::ParseIfExpression));
    prefixParseFns.insert(std::make_pair(TT_FUNCTION, &Parser::ParseFunctionLiteral));
    prefixParseFns.insert(std::make_pair(TT_WHILE, &Parser::ParseWhileExpression));
    prefixParseFns.insert(std::make_pair(TT_FOR, &Parser::ParseForExpression));
    prefixParseFns.insert(std::make_pair(TT_DOUBLE_QUOTES, &Parser::ParseStringLiteral));
    prefixParseFns.insert(std::make_pair(TT_SINGLE_QUOTES, &Parser::ParseStringLiteral));
    prefixParseFns.insert(std::make_pair(TT_LBRACKET, &Parser::ParseListLiteral));
    prefixParseFns.insert(std::make_pair(TT_LBRACE, &Parser::ParseHashLiteral));
    prefixParseFns.insert(std::make_pair(TT_DEF, &Parser::ParseDefExpression));
    prefixParseFns.insert(std::make_pair(TT_CLASS, &Parser::ParseClassStatement));
    prefixParseFns.insert(std::make_pair(TT_PLUSPLUS, &Parser::ParsePrefixPlusPlus));
    prefixParseFns.insert(std::make_pair(TT_NULL, &Parser::ParseNullExpression));
    prefixParseFns.insert(std::make_pair(TT_THIS, &Parser::ParseThisExpression));
    prefixParseFns.insert(std::make_pair(TT_SUPER, &Parser::parseSuperExpression));
    infixParseFns.insert(std::make_pair(TT_PLUS, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_MINUS, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_SLASH, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_ASTERISK, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_EQ, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_NOT_EQ, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_LT, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_GT, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_LTE, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_GTE, &Parser::ParseInfixExpression));
    infixParseFns.insert(std::make_pair(TT_LPAREN, &Parser::ParseCallExpression));
    infixParseFns.insert(std::make_pair(TT_LBRACKET, &Parser::ParseIndexExpression));
    infixParseFns.insert(std::make_pair(TT_ASSIGN, &Parser::ParseAssignExpression));
    infixParseFns.insert(std::make_pair(TT_DOT, &Parser::ParseMemberAccess));
    infixParseFns.insert(std::make_pair(TT_PLUSPLUS, &Parser::ParsePostfixPlusPlus));
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
        Ad_AST_Node *stmt = (Ad_AST_Node*)ParseStatement();
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

PrecedenceType Parser::PeekPrecedence() {
    if (precedences.find(peek_token.type) != precedences.end()) {
        return precedences[peek_token.type];
    }
    return PT_LOWEST;
}

PrecedenceType Parser::CurPrecedence() {
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
    if (current_token.type == TT_BREAK)
        return ParseBreakStatement();
    if (current_token.type == TT_CONTINUE)
        return ParseContinueStatement();
    if (current_token.type == TT_MULTICOMMENT)
        return ParseComment();
    if (current_token.type == TT_SINGLECOMMENT)
        return ParseSingleLineComment();
    return ParseExpressionStatement();
}

Ad_AST_Node* Parser::ParseLetStatement() {
    Ad_AST_LetStatement* stmt = new Ad_AST_LetStatement(current_token);

    if (!ExpectPeek(TT_IDENT)) {
        //delete stmt;
        free_Ad_AST_Node_memory(stmt);
        return NULL;
    }

    stmt->name = Ad_AST_Identifier(current_token, current_token.literal);

    if (!ExpectPeek(TT_ASSIGN)) {
        //delete stmt;
        free_Ad_AST_Node_memory(stmt);
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
    if (PeekTokenIs(TT_RBRACE) || PeekTokenIs(TT_SEMICOLON) || PeekTokenIs(TT_EOF)) {
        return stmt;
    }
    while (!CurrentTokenIs(TT_SEMICOLON) && !CurrentTokenIs(TT_EOF)) {
        NextToken();
    }
    return stmt;
}

Ad_AST_Node* Parser::ParseExpressionStatement() {
    Ad_AST_ExpressionStatement* stmt = new Ad_AST_ExpressionStatement(current_token);
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
    PrecedenceType preced = CurPrecedence();
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

Ad_AST_Node* Parser::ParseFloatLiteral() {
    Ad_AST_Float* float_node = new Ad_AST_Float();
    float val = std::stof(current_token.literal);
    float_node->token = current_token;
    float_node->value = val;
    return float_node;
}

Ad_AST_Node* Parser::ParsePrefixExpression() {
    Ad_AST_PrefixExpression* expr = new Ad_AST_PrefixExpression(current_token, current_token.literal);
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
        //delete expr;
        free_Ad_AST_Node_memory(expr);
        return NULL;
    }
    return expr;
}

Ad_AST_Node* Parser::ParseIndexExpression(Ad_AST_Node* left) {
    Ad_AST_IndexExpression* expr = new Ad_AST_IndexExpression(current_token, left);
    NextToken();
    expr->index = ParseExpression(PT_LOWEST);
    if (!ExpectPeek(TT_RBRACKET)) {
        return NULL;
    }
    return expr;
}

Ad_AST_Node* Parser::ParseIfExpression() {
    Ad_AST_IfExpression* expr = new Ad_AST_IfExpression(current_token);
    if (!ExpectPeek(TT_LPAREN)) {
        free_Ad_AST_Node_memory(expr);
        return NULL;
    }
    NextToken();
    expr->condition = ParseExpression(PT_LOWEST);
    if (!ExpectPeek(TT_RPAREN)) {
        free_Ad_AST_Node_memory(expr);
        return NULL;
    }
    if (!ExpectPeek(TT_LBRACE)) {
        expr->consequence = ParseSingleBlockStatement();
    } else {
        expr->consequence = ParseBlockStatement();
    }

    if (PeekTokenIs(TT_ELSE)) {
        NextToken();
        if (!ExpectPeek(TT_LBRACE)) {
            expr->alternative = ParseSingleBlockStatement();
        } else {
            expr->alternative = ParseBlockStatement();
        }
    }
    return expr;
}

Ad_AST_Node* Parser::ParseFunctionLiteral() {
    Ad_AST_FunctionLiteral* fun_lit = new Ad_AST_FunctionLiteral(current_token);
    if (!ExpectPeek(TT_LPAREN)) {
        //delete fun_lit;
        free_Ad_AST_Node_memory(fun_lit);
        return NULL;
    }
    fun_lit->parameters = ParseFunctionParameters();
    if (!ExpectPeek(TT_LBRACE)) {
        //delete fun_lit;
        free_Ad_AST_Node_memory(fun_lit);
        return NULL;
    }
    fun_lit->body = ParseBlockStatement();
    Ad_INCREF(fun_lit->body);
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
    Ad_INCREF(ident);
    while (PeekTokenIs(TT_COMMA)) {
        NextToken();
        NextToken();
        ident = new Ad_AST_Identifier(current_token, current_token.literal);
        identifiers.push_back(ident);
        Ad_INCREF(ident);
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
        Ad_AST_Node* stmt = (Ad_AST_Node*)ParseStatement();
        if (stmt) {
            block->statements.push_back(stmt);
        }
        NextToken();
    }
    return block;
}

Ad_AST_Node* Parser::ParseSingleBlockStatement() {
    Ad_AST_BlockStatement* block = new Ad_AST_BlockStatement(current_token);
    NextToken();
    Ad_AST_Node* stmt = (Ad_AST_Node*)ParseStatement();
    if (stmt) {
        block->statements.push_back(stmt);
    }
    return block;
}

Ad_AST_Node* Parser::ParseWhileExpression() {
    Ad_AST_WhileExpression* expr = new Ad_AST_WhileExpression(current_token);
    if (!ExpectPeek(TT_LPAREN)) {
        free_Ad_AST_Node_memory(expr);
        return NULL;
    }
    NextToken();
    expr->condition = ParseExpression(PT_LOWEST);
    if (!ExpectPeek(TT_RPAREN)) {
        free_Ad_AST_Node_memory(expr);
        return NULL;
    }
    if (!ExpectPeek(TT_LBRACE)) {
        expr->consequence = ParseSingleBlockStatement();
    } else {
        expr->consequence = ParseBlockStatement();
    }
    return expr;
}

Ad_AST_Node* Parser::ParseStringLiteral() {
    Ad_AST_String *string_node = new Ad_AST_String(current_token);
    string_node->value = current_token.GetLiteral();
    return string_node;
}

Ad_AST_Node* Parser::ParseListLiteral() {
    Ad_AST_ListLiteral* expr = new Ad_AST_ListLiteral(current_token);
    expr->elements = ParseListExpressions();
    return expr;
}

std::vector<Ad_AST_Node*> Parser::ParseListExpressions() {
    std::vector<Ad_AST_Node*> elements;
    if (PeekTokenIs(TT_RBRACKET)) {
        NextToken();
        return elements;
    }
    NextToken();
    Ad_AST_Node* element = ParseExpression(PT_LOWEST);
    //Ad_INCREF(element); // this might be needed
    elements.push_back(element);
    while(PeekTokenIs(TT_COMMA)) {
        NextToken();
        NextToken();
        element = ParseExpression(PT_LOWEST);
        //Ad_INCREF(element); // this might be needed
        elements.push_back(element);
    }
    if (!ExpectPeek(TT_RBRACKET)) {
        std::vector<Ad_AST_Node*> empty;
        return empty;
    }
    return elements;
}

Ad_AST_Node* Parser::ParseHashLiteral() {
    Ad_AST_HashLiteral* hash = new Ad_AST_HashLiteral(current_token);
    while (!PeekTokenIs(TT_RBRACE)) {
        NextToken();
        Ad_AST_Node* key = ParseExpression(PT_LOWEST);
        if (!ExpectPeek(TT_COLON)) {
            return NULL;
        }
        NextToken();
        Ad_AST_Node* value = ParseExpression(PT_LOWEST);
        //Ad_INCREF(key);
        //Ad_INCREF(value);
        hash->pairs.insert(std::make_pair(key, value));
        if (!PeekTokenIs(TT_RBRACE) && !ExpectPeek(TT_COMMA)) {
            return NULL;
        }
    }
    if (!ExpectPeek(TT_RBRACE)) {
        return NULL;
    }
    return hash;
}

Ad_AST_Node* Parser::ParseAssignExpression(Ad_AST_Node* left) {
    Ad_AST_AssignStatement* stmt = new Ad_AST_AssignStatement(current_token);
    stmt->name = left;
    NextToken();
    stmt->value = ParseExpression(PT_LOWEST);
    if (CurrentTokenIs(TT_SEMICOLON)) {
        NextToken();
    }
    return stmt;
}

Ad_AST_Node* Parser::ParseDefExpression() {
    Ad_AST_Def_Statement* stmt = new Ad_AST_Def_Statement(current_token);
    NextToken();
    Ad_AST_Identifier* name = new Ad_AST_Identifier(current_token, current_token.GetLiteral());
    stmt->name = name;
    if (!ExpectPeek(TT_LPAREN)) {
        // free Ad_AST_Def_Statement
        return NULL; // this should potentially return an error AST node
    }
    std::vector<Ad_AST_Node*> parameters = ParseFunctionParameters();
    stmt->parameters = parameters;
    if (!ExpectPeek(TT_LBRACE)) {
        // free As_AST_Def_Statement
        return NULL; // this hsould potentially return an error AST node
    }
    Ad_AST_Node* body = ParseBlockStatement();
    stmt->body = body;
    Ad_INCREF(stmt->body);

    // ****
    if (USE_FIX_FOR_CLASS_STATEMENT_AS_EXPRESSION_STATEMENT_IN_BLOCK) {
        for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_BlockStatement*)body)->statements.begin() ; it != ((Ad_AST_BlockStatement*)body)->statements.end(); ++it) {
            Ad_AST_Node *node = *it;
            //free_Ad_AST_Node_memory(node);
            //std::cout << statement_type_map[node->type] << "\n";
            //Ad_INCREF(node);
            if (node->type == ST_EXPRESSION_STATEMENT) {
                Ad_AST_ExpressionStatement* expr = (Ad_AST_ExpressionStatement*) node;
                if (expr->expression != NULL) {
                    if (expr->expression->type == ST_CLASS_STATEMENT) {
                        //std::cout << "aaa\n";
                        Ad_INCREF(expr->expression);
                    }
                }
            }
        }
    }
    // ****

    return stmt;
}

Ad_AST_Node* Parser::ParseComment() {
    Ad_AST_Comment *comm = new Ad_AST_Comment(current_token);
    return comm;
}

Ad_AST_Node* Parser::ParseSingleLineComment() {
    Ad_AST_Comment *comm = new Ad_AST_Comment(current_token);
    return comm;
}

Ad_AST_Node* Parser::ParseClassStatement() {
    Ad_AST_Class* expr = new Ad_AST_Class(current_token);
    NextToken();
    Ad_AST_Node* name = ParseIdentifier();
    expr->name = name;
    NextToken();
    if (CurrentTokenIs(TT_COLON)) {
        NextToken();
        while (!CurrentTokenIs(TT_LBRACE)) {
            Ad_AST_Node* parent = ParseIdentifier();
            expr->inheritFrom.push_back(parent);
            NextToken();
        }
    }
    while(!CurrentTokenIs(TT_RBRACE)) {
        if (CurrentTokenIs(TT_DEF)) {
            Ad_AST_Node* stmt = ParseDefExpression();
            Ad_INCREF(stmt);
            expr->methods.push_back(stmt);
        }
        if (CurrentTokenIs(TT_IDENT)) {
            Ad_AST_Node* stmt = ParseExpressionStatement();
            Ad_INCREF(stmt);
            expr->attributes.push_back(stmt);
        }
        NextToken();
    }
    return expr;
}

Ad_AST_Node* Parser::ParseMemberAccess(Ad_AST_Node* left) {
    Ad_AST_MemberAccess* stmt = new Ad_AST_MemberAccess(current_token);
    NextToken();
    Ad_AST_Identifier* right = new Ad_AST_Identifier(current_token, current_token.GetLiteral());
    stmt->owner = left;
    stmt->member = right;
    if (PeekTokenIs(TT_LPAREN)) {
        NextToken();
        stmt->arguments = ParseCallArguments();
        stmt->is_method = true;
    } else {
        stmt->arguments.clear();
        stmt->is_method = false;
    }
    return stmt;
}

Ad_AST_Node* Parser::ParsePrefixPlusPlus() {
    Ad_AST_PrefixIncrement* expr = new Ad_AST_PrefixIncrement(current_token);
    NextToken();
    Ad_AST_Identifier* name = new Ad_AST_Identifier(current_token, current_token.GetLiteral());
    expr->name = name;
    return expr;
}

Ad_AST_Node* Parser::ParsePostfixPlusPlus(Ad_AST_Node* left) {
    Ad_AST_PostfixIncrement* expr = new Ad_AST_PostfixIncrement(current_token);
    expr->name = left;
    return expr;
}

Ad_AST_Node* Parser::ParseForExpression() {
    Ad_AST_ForExprssion* expr = new Ad_AST_ForExprssion(current_token);
    NextToken();
    if (!CurrentTokenIs(TT_LPAREN)) {
        std::cout << "error parsing for expression: LPAREN expected\n";
        free_Ad_AST_Node_memory(expr);
        return NULL;
    }
    NextToken();
    expr->initialization = ParseExpression(PT_LOWEST);
    if (!ExpectPeek(TT_SEMICOLON)) {
        std::cout << "error parsing initialization for expression: SEMICOLON expected\n";
        free_Ad_AST_Node_memory(expr);
        return NULL;
    }
    NextToken();
    expr->condition = ParseExpression(PT_LOWEST);
    if (!ExpectPeek(TT_SEMICOLON)) {
        std::cout << "error parsing condition for expression: second SEMICOLON expected\n";
        free_Ad_AST_Node_memory(expr);
        return NULL;
    }
    NextToken();
    expr->step = ParseExpression(PT_LOWEST);
    if (!ExpectPeek(TT_RPAREN)) {
        std::cout << "error parsing step for expression: RPAREN expected\n";
        free_Ad_AST_Node_memory(expr);
        return NULL;
    }
    if (!ExpectPeek(TT_LBRACE)) {
        expr->body = ParseSingleBlockStatement();
    } else {
        expr->body = ParseBlockStatement();
    }
    return expr;
}

Ad_AST_Node* Parser::ParseBreakStatement() {
    Ad_AST_BreakStatement *stmt = new Ad_AST_BreakStatement(current_token);
    return stmt;
}

Ad_AST_Node* Parser::ParseContinueStatement() {
    Ad_AST_ContinueStatement *stmt = new Ad_AST_ContinueStatement(current_token);
    return stmt;
}

Ad_AST_Node* Parser::ParseNullExpression() {
    Ad_AST_Null_Expression* expr = new Ad_AST_Null_Expression(current_token);
    return expr;
}

Ad_AST_Node* Parser::ParseThisExpression() {
    Ad_AST_This_Expression* expr = new Ad_AST_This_Expression(current_token);
    return expr;
}

Ad_AST_Node* Parser::parseSuperExpression() {
    Ad_AST_Super_Expression *expr = new Ad_AST_Super_Expression(current_token);
    NextToken();
    if (!CurrentTokenIs(TT_LPAREN)) {
        return NULL;
    }
    NextToken();
    Ad_AST_Node *target = ParseIdentifier();
    expr->target = target;
    if (!ExpectPeek(TT_RPAREN)) {
        return NULL;
    }
    return expr;
}

Ad_AST_Node* Parser::ParseExpression(PrecedenceType precedence) {
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