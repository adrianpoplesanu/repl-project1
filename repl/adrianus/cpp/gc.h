#ifndef __GC_H
#define __GC_H

#include "environment.h"
#include "objects.h"
#include "ast.h"

class GarbageCollector {
public:
    std::vector<Environment*> garbage_collect_environments;
    std::vector<Ad_Object*> garbage_collect_objects;
    std::vector<Ad_AST_Node*> garbage_collect_ast_nodes;

    GarbageCollector();
    ~GarbageCollector();
    void addEnvironment(Environment*);
    void addObject(Ad_Object*);
    void addAstNode(Ad_AST_Node*);
    void clearEnvironments();
    void clearObjects();
    void clearAstNodes();
    void sweepEnvironments();
    void sweepObjects();
    void sweepAstNodes();
};

#endif