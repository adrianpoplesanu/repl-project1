from token_type import TokenType

def print_ast_node(node, level):
    print ' ' * level,
    if (node.type == TokenType.LET):
        pass
    if (node.type == TokenType.RETURN):
        pass
    else:
        pass