#include <iostream>
#include "ast.h"

#define padding 2

void print_level(int level) {
    for(int i = 0; i < level; i++) std::cout << " ";
}

void print_ast_nodes(Ad_AST_Node* node, int level) {
    switch(node->type) {
        case ST_PROGRAM:
            print_level(level);
            std::cout << "Program\n";
            for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_Program*)node)->statements.begin() ; it != ((Ad_AST_Program*)node)->statements.end(); ++it) {
                Ad_AST_Node *obj = *it;
                print_ast_nodes(obj, level + padding);
            }
        break;
        case ST_LET_STATEMENT:
            print_level(level);
            std::cout << "->LetStatement " << ((Ad_AST_LetStatement*)node)->token.ToString() << " " << ((Ad_AST_LetStatement*)node)->name.token.ToString() << "\n";
            print_ast_nodes(((Ad_AST_LetStatement*)node)->value, level + padding);
        break;
        case ST_RETURN_STATEMENT:
            print_level(level);
            std::cout << "->ReturnStatement " << ((Ad_AST_LetStatement*)node)->token.ToString() << "\n";
            print_ast_nodes(((Ad_AST_ReturnStatement*)node)->value, level + padding);
        break;
        case ST_EXPRESSION_STATEMENT:
            print_level(level);
            std::cout << "->ExpressionStatement " << ((Ad_AST_ExpressionStatement*)node)->token.ToString() << "\n";
            if (((Ad_AST_ExpressionStatement*)node)->expression) {
                print_ast_nodes(((Ad_AST_ExpressionStatement*)node)->expression, level + padding);
            }
        break;
        case ST_IDENTIFIER:
            print_level(level);
            std::cout << "->Identifier " << ((Ad_AST_Identifier*)node)->token.ToString() << "\n";
        break;
        case ST_INTEGER:
            print_level(level);
            std::cout << "->Integer " << ((Ad_AST_Integer*)node)->token.ToString() << "\n";
        break;
        case ST_BOOLEAN:
            print_level(level);
            std::cout << "->Boolean " << ((Ad_AST_Boolean*)node)->token.ToString() << "\n";
        break;
        case ST_INFIX_EXPRESSION:
            print_level(level);
            std::cout << "->InfixExpression " << ((Ad_AST_InfixExpression*)node)->token.ToString() << "\n";
            if (((Ad_AST_InfixExpression*)node)->left) {
                print_ast_nodes(((Ad_AST_InfixExpression*)node)->left, level + padding);
            }
            if (((Ad_AST_InfixExpression*)node)->right) {
                print_ast_nodes(((Ad_AST_InfixExpression*)node)->right, level + padding);
            }
        break;
        case ST_PREFIX_EXPRESSION:
            print_level(level);
            std::cout << "->PrefixExpression " << ((Ad_AST_PrefixExpression*)node)->token.ToString() << "\n";
            if (((Ad_AST_PrefixExpression*)node)->right) {
                print_ast_nodes(((Ad_AST_PrefixExpression*)node)->right, level + padding);
            }
        break;
        case ST_CALL_EXPRESSION:
            print_level(level);
            std::cout << "->CallExpression " << ((Ad_AST_CallExpression*)node)->token.ToString() << "\n";
            print_ast_nodes(((Ad_AST_CallExpression*)node)->function, level + padding);
            for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_CallExpression*)node)->arguments.begin() ; it != ((Ad_AST_CallExpression*)node)->arguments.end(); ++it) {
                Ad_AST_Node *obj = *it;
                print_ast_nodes(obj, level + padding);
            }
        break;
        case ST_IF_EXPRESSION:
            print_level(level);
            std::cout << "->IfExpression " << ((Ad_AST_IfExpression*)node)->token.ToString() << "\n";
            print_ast_nodes(((Ad_AST_IfExpression*)node)->condition, level + padding);
            print_ast_nodes(((Ad_AST_IfExpression*)node)->consequence, level + padding);
            print_ast_nodes(((Ad_AST_IfExpression*)node)->alternative, level + padding);
        break;
        case ST_BLOCK_STATEMENT:
            print_level(level);
            std::cout << "->BlockStatement " << ((Ad_AST_BlockStatement*)node)->token.ToString() << "\n";
            for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_BlockStatement*)node)->statements.begin() ; it != ((Ad_AST_BlockStatement*)node)->statements.end(); ++it) {
                Ad_AST_Node *obj = *it;
                print_ast_nodes(obj, level + padding);
            }
        break;
        case ST_FUNCTION_LITERAL:
            print_level(level);
            std::cout << "->FunctionLiteral " << ((Ad_AST_FunctionLiteral*)node)->token.ToString() << "\n";
            for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_FunctionLiteral*)node)->parameters.begin() ; it != ((Ad_AST_FunctionLiteral*)node)->parameters.end(); ++it) {
                Ad_AST_Node *obj = *it;
                print_ast_nodes(obj, level + padding);
            }
            print_ast_nodes(((Ad_AST_FunctionLiteral*)node)->body, level + padding);
        break;
        case ST_WHILE_EXPRESSION:
            print_level(level);
            std::cout << "->WhileExpression " << ((Ad_AST_WhileExpression*)node)->token.ToString() << "\n";
            print_ast_nodes(((Ad_AST_WhileExpression*)node)->condition, level + padding);
            print_ast_nodes(((Ad_AST_WhileExpression*)node)->consequence, level + padding);
        break;
        case ST_STRING_LITERAL:
            print_level(level);
            std::cout << "->StringLiteral " << ((Ad_AST_String*)node)->token.ToString() << "\n";
        break;
    }
}