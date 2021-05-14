#ifndef __AST_H
#define __AST_H

#include <string>
#include <vector>

class Ad_AST_Node {
public:
    std::string TokenLiteral();
    std::string String();
};

class Ad_AST_Statement : public Ad_AST_Node {
public:
    std::string TokenLiteral();
    std::string String();
};

class Ad_AST_Expression : public Ad_AST_Node {
public:
    std::string TokenLiteral();
    std::string String();
};

class Ad_AST_Program : public Ad_AST_Node {
public:
    std::vector<Ad_AST_Node*> statements;

    std::string TokenLiteral();
    std::string String();
};

#endif