#include "ast.h"
#include <iostream>

std::string Ad_AST_Node::TokenLiteral() {
    return "node literal - implement in subclass";
}

std::string Ad_AST_Node::ToString() {
    return "node string repr - implement in subclass";
}

Ad_AST_Program::Ad_AST_Program() {
    type = ST_PROGRAM;
    ref_count = 0;
}

Ad_AST_Program::~Ad_AST_Program() {
    for (std::vector<Ad_AST_Node*>::iterator it = statements.begin() ; it != statements.end(); ++it) {
        Ad_AST_Node *node = *it;
        free_Ad_AST_Node_memory(node);
    }
}

std::string Ad_AST_Program::TokenLiteral() {
    return "todo - implement this";
}

std::string Ad_AST_Program::ToString() {
    std::string out = "";
    for (std::vector<Ad_AST_Node*>::iterator it = statements.begin() ; it != statements.end(); ++it) {
        Ad_AST_Node *stmt = *it;
        if (stmt) {
            std::cout << stmt->ToString() << "\n";
        }
    }
    return out;
}

void Ad_AST_Program::reset() {
    for (std::vector<Ad_AST_Node*>::iterator it = statements.begin() ; it != statements.end(); ++it) {
        Ad_AST_Node *node = *it;
        free_Ad_AST_Node_memory(node);
    }
    statements.clear();
}

Ad_AST_LetStatement::Ad_AST_LetStatement() {
    type = ST_LET_STATEMENT;
    ref_count = 0;
    value = NULL;
}

Ad_AST_LetStatement::Ad_AST_LetStatement(Token t) {
    type = ST_LET_STATEMENT;
    ref_count = 0;
    token = t;
    name = Ad_AST_Identifier();
    value = NULL;
}

Ad_AST_LetStatement::~Ad_AST_LetStatement() {
    if (value) {
        free_Ad_AST_Node_memory(value);
    }
}

std::string Ad_AST_LetStatement::TokenLiteral() {
    return "";
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

Ad_AST_AssignStatement::Ad_AST_AssignStatement() {
    type = ST_ASSIGN_STATEMENT;
    ref_count = 0;
    value = NULL;
}

Ad_AST_AssignStatement::Ad_AST_AssignStatement(Token t) {
    type = ST_ASSIGN_STATEMENT;
    ref_count = 0;
    token = t;
    //name = new Ad_AST_Identifier();
    name = NULL;
    value = NULL;
}

Ad_AST_AssignStatement::~Ad_AST_AssignStatement() {
    if (name) {
        free_Ad_AST_Node_memory(name);
    }
    if (value) {
        free_Ad_AST_Node_memory(value);
    }
}

std::string Ad_AST_AssignStatement::TokenLiteral() {
    return "";
}

std::string Ad_AST_AssignStatement::ToString() {
    std::string out = "AssignStatement [";
    out +=  token.literal + "] <" + ((Ad_AST_Identifier*)name)->value + ">: ";
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
    ref_count = 0;
    value = NULL;
}

Ad_AST_ReturnStatement::Ad_AST_ReturnStatement(Token t) {
    type = ST_RETURN_STATEMENT;
    ref_count = 0;
    token = t;
    value = NULL;
}

Ad_AST_ReturnStatement::~Ad_AST_ReturnStatement() {
    free_Ad_AST_Node_memory(value);
}

std::string Ad_AST_ReturnStatement::TokenLiteral() {
    return "";
}

std::string Ad_AST_ReturnStatement::ToString() {
    std::string out = "ReturnStatement [";
    out +=  token.literal + "] <" + value->ToString() + ">";
    return out;
}

Ad_AST_ExpressionStatement::Ad_AST_ExpressionStatement() {
    type = ST_EXPRESSION_STATEMENT;
    ref_count = 0;
    expression = NULL;
}

Ad_AST_ExpressionStatement::Ad_AST_ExpressionStatement(Token t) {
    type = ST_EXPRESSION_STATEMENT;
    ref_count = 0;
    token = t;
    expression = NULL;
}

Ad_AST_ExpressionStatement::~Ad_AST_ExpressionStatement() {
    if (expression) { // TODO: de ce trebuie asta? nu inteleg de ce trebuie if-ul asta aici
        free_Ad_AST_Node_memory(expression);
    }
}

std::string Ad_AST_ExpressionStatement::TokenLiteral() {
    return "";
}

std::string Ad_AST_ExpressionStatement::ToString() {
    std::string out = "ExpressionStatement [";
    out +=  token.literal + "] <";
    if (expression) {
        out += expression->ToString();
    } else {
        out += "null";
    }
    out += ">";
    return out;
}

Ad_AST_Identifier::Ad_AST_Identifier() {
    type = ST_IDENTIFIER;
    ref_count = 0;
}

Ad_AST_Identifier::Ad_AST_Identifier(Token t, std::string val) {
    type = ST_IDENTIFIER;
    ref_count = 0;
    token = t;
    value = val;
}

std::string Ad_AST_Identifier::TokenLiteral() {
    return token.GetLiteral();
}

std::string Ad_AST_Identifier::ToString() {
    return value;
}

Ad_AST_Integer::Ad_AST_Integer() {
    type = ST_INTEGER;
    ref_count = 0;
}

Ad_AST_Integer::Ad_AST_Integer(Token t, int val) {
    type = ST_INTEGER;
    ref_count = 0;
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
    ref_count = 0;
}

Ad_AST_Boolean::Ad_AST_Boolean(Token t, bool val) {
    type = ST_BOOLEAN;
    ref_count = 0;
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
    ref_count = 0;
    left = NULL;
    right = NULL;
}

Ad_AST_InfixExpression::~Ad_AST_InfixExpression() {
    free_Ad_AST_Node_memory(left);
    free_Ad_AST_Node_memory(right);
}

std::string Ad_AST_InfixExpression::ToString() {
    return "(" + left->ToString() + _operator + right->ToString() + ")";
}

Ad_AST_PrefixExpression::Ad_AST_PrefixExpression() {
    type = ST_PREFIX_EXPRESSION;
    ref_count = 0;
    right = NULL;
}

Ad_AST_PrefixExpression::Ad_AST_PrefixExpression(Token t, std::string op) {
    type = ST_PREFIX_EXPRESSION;
    ref_count = 0;
    token = t;
    _operator = op;
    right = NULL;
}

Ad_AST_PrefixExpression::~Ad_AST_PrefixExpression() {
    free_Ad_AST_Node_memory(right);
}

std::string Ad_AST_PrefixExpression::ToString() {
    std::string out;
    out = "(" + _operator + right->ToString() + ")";
    return out;
}

Ad_AST_CallExpression::Ad_AST_CallExpression() {
    type = ST_CALL_EXPRESSION;
    ref_count = 0;
    function = NULL;
}

Ad_AST_CallExpression::Ad_AST_CallExpression(Token t) {
    type = ST_CALL_EXPRESSION;
    ref_count = 0;
    token = t;
    function = NULL;
}

Ad_AST_CallExpression::Ad_AST_CallExpression(Token t, Ad_AST_Node* f) {
    type = ST_CALL_EXPRESSION;
    ref_count = 0;
    token = t;
    function = f;
}

Ad_AST_CallExpression::~Ad_AST_CallExpression() {
    free_Ad_AST_Node_memory(function);
    for (std::vector<Ad_AST_Node*>::iterator it = arguments.begin() ; it != arguments.end(); ++it) {
        Ad_AST_Node *node = *it;
        free_Ad_AST_Node_memory(node);
    }
}

std::string Ad_AST_CallExpression::ToString() {
    std::string out = "[CallExpression] ";
    out += function->ToString();
    out += "(";
    for (std::vector<Ad_AST_Node*>::iterator it = arguments.begin() ; it != arguments.end(); ++it) {
        Ad_AST_Node *current = *it;
        if (it != arguments.begin()) out += ", ";
        out += current->ToString();
    }
    out += ")";
    return out;
}

Ad_AST_IfExpression::Ad_AST_IfExpression() {
    type = ST_IF_EXPRESSION;
    ref_count = 0;
    condition = NULL;
    consequence = NULL;
    alternative = NULL;
}

Ad_AST_IfExpression::Ad_AST_IfExpression(Token t) {
    type = ST_IF_EXPRESSION;
    ref_count = 0;
    token = t;
    condition = NULL;
    consequence = NULL;
    alternative = NULL;
}

Ad_AST_IfExpression::~Ad_AST_IfExpression() {
    if (condition) {
        free_Ad_AST_Node_memory(condition);
    }
    if (consequence) {
        free_Ad_AST_Node_memory(consequence);
    }
    if (alternative) {
        free_Ad_AST_Node_memory(alternative);
    }
}

std::string Ad_AST_IfExpression::ToString() {
    std::string out = "";
    out += "[IfExpression] ";
    if (condition) {
        out += "condition: " + condition->ToString() + " ";
    }
    if (consequence) {
        out += "consequence: " + consequence->ToString() + " ";
    }
    if (alternative) {
        out += "alternative: " + alternative->ToString() + " ";
    }
    return out;
}

Ad_AST_BlockStatement::Ad_AST_BlockStatement() {
    type = ST_BLOCK_STATEMENT;
    ref_count = 0;
}

Ad_AST_BlockStatement::Ad_AST_BlockStatement(Token t) {
    type = ST_BLOCK_STATEMENT;
    ref_count = 0;
    token = t;
}

Ad_AST_BlockStatement::~Ad_AST_BlockStatement() {
    for (std::vector<Ad_AST_Node*>::iterator it = statements.begin() ; it != statements.end(); ++it) {
        Ad_AST_Node *node = *it;
        free_Ad_AST_Node_memory(node);
    }
}

std::string Ad_AST_BlockStatement::ToString() {
    std::string out = "";
    for (std::vector<Ad_AST_Node*>::iterator it = statements.begin() ; it != statements.end(); ++it) {
        Ad_AST_Node *node = *it;
        out += node->ToString();
    }
    return out;
}

Ad_AST_FunctionLiteral::Ad_AST_FunctionLiteral() {
    type = ST_FUNCTION_LITERAL;
    ref_count = 0; // poate aici ar trebui sa fie 1
}

Ad_AST_FunctionLiteral::Ad_AST_FunctionLiteral(Token t) {
    type = ST_FUNCTION_LITERAL;
    ref_count = 0; // poate aici ar trebui sa fie 1
    token = t;
}

Ad_AST_FunctionLiteral::~Ad_AST_FunctionLiteral() {
    if (body) {
        Ad_DECREF(body); // asta merge si e super cool
        free_Ad_AST_Node_memory(body);
    }
    for (std::vector<Ad_AST_Node*>::iterator it = parameters.begin() ; it != parameters.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_DECREF(node); // asta merge si e super cool
        free_Ad_AST_Node_memory(node);
    }
}

std::string Ad_AST_FunctionLiteral::ToString() {
    std::string out = "[FunctionLiteral] <";
    for (std::vector<Ad_AST_Node*>::iterator it = parameters.begin() ; it != parameters.end(); ++it) {
        Ad_AST_Node *current = *it;
        if (it != parameters.begin()) out += ", ";
        out += current->ToString();
    }
    out += "><";
    out += body->ToString();
    out += ">";
    return out;
}

Ad_AST_WhileExpression::Ad_AST_WhileExpression() {
    type = ST_WHILE_EXPRESSION;
    ref_count = 0;
}

Ad_AST_WhileExpression::Ad_AST_WhileExpression(Token t) {
    type = ST_WHILE_EXPRESSION;
    ref_count = 0;
    token = t;
}

Ad_AST_WhileExpression::~Ad_AST_WhileExpression() {
    if (condition) {
        free_Ad_AST_Node_memory(condition);
    }
    if (consequence) {
        free_Ad_AST_Node_memory(consequence);
    }
}

std::string Ad_AST_WhileExpression::TokenLiteral() {
    return "";
}

std::string Ad_AST_WhileExpression::ToString() {
    std::string out = "";
    return out;
}

Ad_AST_String::Ad_AST_String() {
    type = ST_STRING_LITERAL;
    ref_count = 0;
}

Ad_AST_String::Ad_AST_String(Token t) {
    type = ST_STRING_LITERAL;
    ref_count = 0;
    token = t;
}

Ad_AST_String::~Ad_AST_String() {

}

std::string Ad_AST_String::TokenLiteral() {
    return token.GetLiteral();
}

std::string Ad_AST_String::ToString() {
    std::string out = "";
    return out;
}

Ad_AST_ListLiteral::Ad_AST_ListLiteral() {
    type = ST_LIST_LITERAL;
    ref_count = 0;
}

Ad_AST_ListLiteral::Ad_AST_ListLiteral(Token t) {
    type = ST_LIST_LITERAL;
    ref_count = 0;
    token = t;
}

Ad_AST_ListLiteral::~Ad_AST_ListLiteral() {
    for (std::vector<Ad_AST_Node*>::iterator it = elements.begin() ; it != elements.end(); ++it) {
        Ad_AST_Node *node = *it;
        free_Ad_AST_Node_memory(node);
    }
}

std::string Ad_AST_ListLiteral::TokenLiteral() {
    return token.GetLiteral();
}

std::string Ad_AST_ListLiteral::ToString() {
    std::string out = "[";
    for (std::vector<Ad_AST_Node*>::iterator it = elements.begin() ; it != elements.end(); ++it) {
        Ad_AST_Node *current = *it;
        out += current->ToString() + ", ";
    }
    out += "]";
    return out;
}

Ad_AST_IndexExpression::Ad_AST_IndexExpression() {
    type = ST_INDEX_EXPRESSION;
    ref_count = 0;
}

Ad_AST_IndexExpression::Ad_AST_IndexExpression(Token t) {
    type = ST_INDEX_EXPRESSION;
    ref_count = 0;
    token = t;
}

Ad_AST_IndexExpression::Ad_AST_IndexExpression(Token t, Ad_AST_Node* l) {
    type = ST_INDEX_EXPRESSION;
    ref_count = 0;
    token = t;
    left = l;
}

Ad_AST_IndexExpression::~Ad_AST_IndexExpression() {
    free_Ad_AST_Node_memory(left);
    free_Ad_AST_Node_memory(index);
}

std::string Ad_AST_IndexExpression::TokenLiteral() {
    return "todo: implement Ad_AST_IndexExpression TokenLiteral()";
}

std::string Ad_AST_IndexExpression::ToString() {
    return "todo: implement Ad_AST_IndexExpression ToString()";
}


Ad_AST_HashLiteral::Ad_AST_HashLiteral() {
    type = ST_HASH_LITERAL;
    ref_count = 0;
}

Ad_AST_HashLiteral::Ad_AST_HashLiteral(Token t) {
    type = ST_HASH_LITERAL;
    ref_count = 0;
    token = t;
}

Ad_AST_HashLiteral::Ad_AST_HashLiteral(Token t, std::map<Ad_AST_Node*, Ad_AST_Node*> p) {
    type = ST_HASH_LITERAL;
    ref_count = 0;
    token = t;
    pairs = p;
}

Ad_AST_HashLiteral::~Ad_AST_HashLiteral() {
    for(std::map<Ad_AST_Node*, Ad_AST_Node*>::iterator it = pairs.begin(); it != pairs.end(); it++) {
        Ad_DECREF(it->first);
        Ad_DECREF(it->second);
        free_Ad_AST_Node_memory(it->first);
        free_Ad_AST_Node_memory(it->second);
    }
}

std::string Ad_AST_HashLiteral::TokenLiteral() {
    return "todo: implement Ad_AST_HashLiteral TokenLiteral()";
}

std::string Ad_AST_HashLiteral::ToString() {
    return "todo: implement Ad_AST_HashLiteral ToString()";
}

Ad_AST_Def_Statement::Ad_AST_Def_Statement() {
    type = ST_DEF_STATEMENT;
    ref_count = 0;
}

Ad_AST_Def_Statement::Ad_AST_Def_Statement(Token t) {
    type = ST_DEF_STATEMENT;
    ref_count = 0;
    token = t;
}

Ad_AST_Def_Statement::~Ad_AST_Def_Statement() {
    if (name) {
        Ad_DECREF(name);
        free_Ad_AST_Node_memory(name);
    }
    if (body) {
        Ad_DECREF(body); // asta merge si e super cool
        free_Ad_AST_Node_memory(body);
    }
    for (std::vector<Ad_AST_Node*>::iterator it = parameters.begin() ; it != parameters.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_DECREF(node); // asta merge si e super cool
        free_Ad_AST_Node_memory(node);
    }
}

std::string Ad_AST_Def_Statement::TokenLiteral() {
    return "todo: implement Ad_AST_Def_Statement TokenLiteral()";
}

std::string Ad_AST_Def_Statement::ToString() {
    return "todo: implement Ad_AST_Def_Statement ToString()";
}

Ad_AST_Comment::Ad_AST_Comment() {
    type = ST_COMMENT;
    ref_count = 0;
}

Ad_AST_Comment::Ad_AST_Comment(Token t) {
    type = ST_COMMENT;
    ref_count = 0;
    token = t;
}

Ad_AST_Comment::~Ad_AST_Comment() {

}

std::string Ad_AST_Comment::TokenLiteral() {
    return token.GetLiteral();
}

std::string Ad_AST_Comment::ToString() {
    return "todo: implement Ad_AST_Comment ToString()";
}

Ad_AST_Class::Ad_AST_Class() {
    type = ST_CLASS_STATEMENT;
    ref_count = 0;
}

Ad_AST_Class::Ad_AST_Class(Token t) {
    type = ST_CLASS_STATEMENT;
    ref_count = 0;
    token = t;
}

Ad_AST_Class::~Ad_AST_Class() {
    // Class objects need a reference to this AST node that they will deallocate on object destruction
    if (name) {
        Ad_DECREF(name);
        free_Ad_AST_Node_memory(name);
    }
    for (std::vector<Ad_AST_Node*>::iterator it = methods.begin() ; it != methods.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_DECREF(node); // asta merge si e super cool
        free_Ad_AST_Node_memory(node);
    }
    for (std::vector<Ad_AST_Node*>::iterator it = attributes.begin() ; it != attributes.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_DECREF(node); // asta merge si e super cool
        free_Ad_AST_Node_memory(node);
    }
}

std::string Ad_AST_Class::TokenLiteral() {
    return token.GetLiteral();
}

std::string Ad_AST_Class::ToString() {
    return "todo: implement Ad_AST_Class ToString()";
}

Ad_AST_MemberAccess::Ad_AST_MemberAccess() {
    type = ST_MEMBER_ACCESS;
    ref_count = 0;
}

Ad_AST_MemberAccess::Ad_AST_MemberAccess(Token t) {
    type = ST_MEMBER_ACCESS;
    ref_count = 0;
    token = t;
}

Ad_AST_MemberAccess::Ad_AST_MemberAccess(Token t, Ad_AST_Node* o, Ad_AST_Node* m, std::vector<Ad_AST_Node*> a) {
    type = ST_MEMBER_ACCESS;
    ref_count = 0;
    token = t;
    owner = o;
    member = m;
    arguments = a;
}

Ad_AST_MemberAccess::~Ad_AST_MemberAccess() {
    if (owner) {
        Ad_DECREF(owner);
        free_Ad_AST_Node_memory(owner);
    }
    if (member) {
        Ad_DECREF(member);
        free_Ad_AST_Node_memory(member);
    }
    for (std::vector<Ad_AST_Node*>::iterator it = arguments.begin() ; it != arguments.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_DECREF(node); // asta merge si e super cool
        free_Ad_AST_Node_memory(node);
    }
}

std::string Ad_AST_MemberAccess::TokenLiteral() {
    return token.GetLiteral();
}

std::string Ad_AST_MemberAccess::ToString() {
    return "todo: implement Ad_AST_MemberAccess ToString()";
}

Ad_AST_Null_Expression::Ad_AST_Null_Expression() {
    type = ST_NULL_EXPRESSION;
    ref_count = 0;
}

Ad_AST_Null_Expression::Ad_AST_Null_Expression(Token t) {
    type = ST_NULL_EXPRESSION;
    ref_count = 0;
    token = t;
}

Ad_AST_Null_Expression::~Ad_AST_Null_Expression() {
    // nothing to do here, type and ref_count are deallocated automatically(because they are not pointers)
}

Ad_AST_This_Expression::Ad_AST_This_Expression() {
    type = ST_THIS_EXPRESSION;
    ref_count = 0;
}

Ad_AST_This_Expression::Ad_AST_This_Expression(Token t) {
    type = ST_THIS_EXPRESSION;
    ref_count = 0;
    token = t;
}

Ad_AST_This_Expression::~Ad_AST_This_Expression() {
    // nothing to do here, type and ref_count are deallocated automatically(because they are not pointers)
}

Ad_AST_PrefixIncrement::Ad_AST_PrefixIncrement() {
    type = ST_PREFIX_INCREMENT;
    ref_count = 0;
}

Ad_AST_PrefixIncrement::Ad_AST_PrefixIncrement(Token t) {
    type = ST_PREFIX_INCREMENT;
    ref_count = 0;
    token = t;
}

Ad_AST_PrefixIncrement::Ad_AST_PrefixIncrement(Token t, Ad_AST_Node* n) {
    type = ST_PREFIX_INCREMENT;
    ref_count = 0;
    token = t;
    name = n;
}

Ad_AST_PrefixIncrement::~Ad_AST_PrefixIncrement() {
    if (name) {
        Ad_DECREF(name);
        free_Ad_AST_Node_memory(name);
    }
}

std::string Ad_AST_PrefixIncrement::TokenLiteral() {
    return token.GetLiteral();
}

std::string Ad_AST_PrefixIncrement::ToString() {
    return "todo: implement ToString() in Ad_ast_PrefixIncrement";
}

Ad_AST_PostfixIncrement::Ad_AST_PostfixIncrement() {
    type = ST_POSTFIX_INCREMENT;
    ref_count = 0;
}

Ad_AST_PostfixIncrement::Ad_AST_PostfixIncrement(Token t) {
    type = ST_POSTFIX_INCREMENT;
    ref_count = 0;
    token = t;
}

Ad_AST_PostfixIncrement::Ad_AST_PostfixIncrement(Token t, Ad_AST_Node* n) {
    type = ST_POSTFIX_INCREMENT;
    ref_count = 0;
    token = t;
    name = n;
}

Ad_AST_PostfixIncrement::~Ad_AST_PostfixIncrement() {
    if (name) {
        Ad_DECREF(name);
        free_Ad_AST_Node_memory(name);
    }
}

std::string Ad_AST_PostfixIncrement::TokenLiteral() {
    return token.GetLiteral();
}

std::string Ad_AST_PostfixIncrement::ToString() {
    return "todo: implement ToString() in Ad_AST_PostfixIncrement";
}

Ad_AST_ForExprssion::Ad_AST_ForExprssion() {
    type = ST_FOR_EXPRESSION;
    ref_count = 0;
}

Ad_AST_ForExprssion::Ad_AST_ForExprssion(Token t) {
    type = ST_FOR_EXPRESSION;
    ref_count = 0;
    token = t;
}

Ad_AST_ForExprssion::Ad_AST_ForExprssion(Token t, Ad_AST_Node* i, Ad_AST_Node* c, Ad_AST_Node* s, Ad_AST_Node* b) {
    type = ST_FOR_EXPRESSION;
    ref_count = 0;
    token = t;
    initialization = i;
    condition = c;
    step = s;
    body = b;
}

Ad_AST_ForExprssion::~Ad_AST_ForExprssion() {
    //... TODO
}

std::string Ad_AST_ForExprssion::TokenLiteral() {
    return token.GetLiteral();
}

std::string Ad_AST_ForExprssion::ToString() {
    return "todo: implemnt ToString() in Ad_AST_ForExprssion";
}

void Ad_INCREF(Ad_AST_Node* node) {
    if (node) {
        node->ref_count++;
    }
}

void Ad_DECREF(Ad_AST_Node* node){
    if (node) {
        node->ref_count--;
    }
}

void free_Ad_AST_Node_memory(Ad_AST_Node* node) {
    if (node == NULL) return;
    if (node->ref_count > 0) return;
    switch(node->type) {
        case ST_LET_STATEMENT:
            delete (Ad_AST_LetStatement*)node;
        break;
        case ST_ASSIGN_STATEMENT:
            delete (Ad_AST_AssignStatement*)node;
        break;
        case ST_RETURN_STATEMENT:
            delete (Ad_AST_ReturnStatement*)node;
        break;
        case ST_EXPRESSION_STATEMENT:
            delete (Ad_AST_ExpressionStatement*)node;
        break;
        case ST_IDENTIFIER:
            delete (Ad_AST_Identifier*)node;
        break;
        case ST_INTEGER:
            delete (Ad_AST_Integer*)node;
        break;
        case ST_BOOLEAN:
            delete (Ad_AST_Boolean*)node;
        break;
        case ST_INFIX_EXPRESSION:
            delete (Ad_AST_InfixExpression*)node;
        break;
        case ST_PREFIX_EXPRESSION:
            delete (Ad_AST_PrefixExpression*)node;
        break;
        case ST_CALL_EXPRESSION:
            delete (Ad_AST_CallExpression*)node;
        break;
        case ST_IF_EXPRESSION:
            delete (Ad_AST_IfExpression*)node;
        break;
        case ST_BLOCK_STATEMENT:
            delete (Ad_AST_BlockStatement*)node;
        break;
        case ST_FUNCTION_LITERAL:
            delete (Ad_AST_FunctionLiteral*)node;
            // ATENTION: this needs to be deleted only when there's no env referencing it
            // this should be left in the hand of Ad_INCREF/Ad_DECREF logic and free object which deletes the AST nodes contained in Ad_Function_Object
        break;
        case ST_WHILE_EXPRESSION:
            delete (Ad_AST_WhileExpression*)node;
        break;
        case ST_STRING_LITERAL:
            delete (Ad_AST_String*)node;
        break;
        case ST_LIST_LITERAL:
            delete (Ad_AST_ListLiteral*)node;
        break;
        case ST_INDEX_EXPRESSION:
            delete (Ad_AST_IndexExpression*)node;
        break;
        case ST_HASH_LITERAL:
            delete (Ad_AST_HashLiteral*)node;
        break;
        case ST_DEF_STATEMENT:
            delete (Ad_AST_Def_Statement*)node;
        break;
        case ST_COMMENT:
            delete (Ad_AST_Comment*)node;
        break;
        case ST_CLASS_STATEMENT:
            delete (Ad_AST_Class*)node;
        break;
        case ST_MEMBER_ACCESS:
            delete (Ad_AST_MemberAccess*) node;
        break;
        case ST_PREFIX_INCREMENT:
            delete (Ad_AST_PrefixIncrement*) node;
        break;
        case ST_POSTFIX_INCREMENT:
            delete (Ad_AST_PostfixIncrement*) node;
        break;
        case ST_NULL_EXPRESSION:
            delete (Ad_AST_Null_Expression*) node;
        break;
        case ST_THIS_EXPRESSION:
            delete (Ad_AST_This_Expression*) node;
        break;
        default:
            std::cout << "MEMORY ERROR!!! ast: " << statement_type_map[node->type] << "\n";
        break;
    }
}