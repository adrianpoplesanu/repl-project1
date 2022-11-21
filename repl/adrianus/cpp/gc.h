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
    void markEnvironment(Environment*);
    void markObject(Ad_Object*);
    void markAstNode(Ad_AST_Node*);
    void sweepEnvironments();
    void sweepObjects();
    void sweepAstNodes();
};

#endif