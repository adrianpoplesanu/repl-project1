#include <iostream>
#include "ast.h"

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
                print_ast_nodes(obj, level + 2);
            }
        break;
        case ST_LET_STATEMENT:
            print_level(level);
            std::cout << "->LetStatement " << ((Ad_AST_LetStatement*)node)->token.ToString() << " " << ((Ad_AST_LetStatement*)node)->name.token.ToString() << "\n";
            print_ast_nodes(((Ad_AST_LetStatement*)node)->value, level + 2);
        break;
        case ST_RETURN_STATEMENT:
            print_level(level);
            std::cout << "->ReturnStatement " << ((Ad_AST_LetStatement*)node)->token.ToString() << "\n";
            print_ast_nodes(((Ad_AST_ReturnStatement*)node)->value, level + 2);
        break;
        case ST_EXPRESSION_STATEMENT:
            print_level(level);
            std::cout << "->ExpressionStatement " << ((Ad_AST_ExpressionStatement*)node)->token.ToString() << "\n";
            if (((Ad_AST_ExpressionStatement*)node)->expression) {
                print_ast_nodes(((Ad_AST_ExpressionStatement*)node)->expression, level + 2);
            }
        break;
        case ST_IDENTIFIER:
            print_level(level);
            std::cout << "->Identifier\n";
        break;
        case ST_INTEGER:
            print_level(level);
            std::cout << "->Integer " << ((Ad_AST_Integer*)node)->token.ToString() << "\n";
        break;
        case ST_BOOLEAN:
            print_level(level);
            std::cout << "->Boolean " << ((Ad_AST_Boolean*)node)->token.ToString() << "\n";
        break;
        case ST_STATEMENT:
            print_level(level);
            std::cout << "->Statement\n";
        break;
        case ST_EXPRESSION:
            print_level(level);
            std::cout << "->Expression\n";
        break;
        case ST_INFIX_EXPRESSION:
            print_level(level);
            std::cout << "->InfixExpression\n";
        break;
        case ST_PREFIX_EXPRESSION:
            print_level(level);
            std::cout << "->PrefixExpression\n";
        break;
        case ST_CALL_EXPRESSION:
            print_level(level);
            std::cout << "->CallExpression\n";
        break;
        case ST_IF_EXPRESSION:
            print_level(level);
            std::cout << "->IfExpression\n";
        break;
        case ST_BLOCK_STATEMENT:
            print_level(level);
            std::cout << "->BlockStatement\n";
        break;
        case ST_FUNCTION_LITERAL:
            print_level(level);
            std::cout << "->FunctionLiteral " << ((Ad_AST_FunctionLiteral*)node)->token.ToString() << "\n";
        break;
    }
}