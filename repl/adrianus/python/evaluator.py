from object import Ad_Null_Object, Ad_Integer_Object, Ad_Boolean_Object, \
                   Ad_String_Object, Ad_ReturnValue_Object, Ad_Function_Object, \
                   Ad_Error_Object
from ast import StatementType

class Evaluator(object):
    def __init__(self):
        pass

    def eval(self, node, env):
        if node.type == StatementType.PROGRAM:
            return self.eval_program(node, env)
        elif node.type == StatementType.LET_STATEMENT:
            obj = self.eval(node.value, env)
            env.set(node.name.value, obj)
            return None
        elif node.type == StatementType.RETURN_STATEMENT:
            val = self.eval(node.value, env)
            obj = Ad_ReturnValue_Object(value=val)
            return obj
        elif node.type == StatementType.EXPRESSION_STATEMENT:
            if node.expression:
                return self.eval(node.expression, env)
            return None
        elif node.type == StatementType.IDENTIFIER:
            return self.eval_identifier(node, env)
        elif node.type == StatementType.INTEGER:
            return self.eval_integer(node, env)
        elif node.type == StatementType.BOOLEAN:
            return self.eval_boolean(node, env)
        elif node.type == StatementType.INFIX_EXPRESSION:
            left = self.eval(node.left, env)
            right = self.eval(node.right, env)
            result = self.eval_infix_expression(node.operator, left, right)
            return result
        elif node.type == StatementType.PREFIX_EXPRESSION:
            right = self.eval(node.right, env)
            return self.eval_prefix_expression(node.operator, right)
        elif node.type == StatementType.IF_EXPRESSION:
            return self.eval_if_expression(node, env)
        elif node.type == StatementType.BLOCK_STATEMENT:
            return self.eval_block_statement(node, env)
        elif node.type == StatementType.FUNCTION_LITERAL:
            obj = Ad_Function_Object(parameters=node.parameters, body=node.body, env=env)
            return obj
        elif node.type == StatementType.CALL_EXPRESSION:
            pass
        elif node.type == StatementType.WHILE_EXPRESSION:
            pass
        else:
            print 'unknown AST node'

    def eval_program(self, node, env):
        for statement in node.statements:
            result = self.eval(statement, env)
            if result:
                print result.inspect()

    def eval_identifier(self, node, env):
        return env.get(node.token.literal)

    def eval_integer(self, node, env):
        obj = Ad_Integer_Object(value=node.value)
        return obj

    def eval_boolean(self, node, env):
        pass

    def eval_infix_expression(self, operator, left, right):
        pass

    def eval_prefix_expression(self, operator, right):
        pass

    def eval_if_expression(self, node, env):
        pass

    def eval_block_statement(self, node, env):
        pass