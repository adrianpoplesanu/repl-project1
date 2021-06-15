from ast import StatementType

def print_ast_node(node, level):
    print ' ' * level,
    if (node.type == StatementType.PROGRAM):
        pass
    elif (node.type == StatementType.LET_STATEMENT):
        pass
    elif (node.type == StatementType.RETURN_STATEMENT):
        pass
    else:
        print 'ERROR: print ast node unknow type'