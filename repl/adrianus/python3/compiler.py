from code.instructions import Instructions
from objects import Ad_Integer_Object


class Compiler:
    def __init__(self):
        self.instructions = Instructions()
        constants = []

    def new(self):
        self.instructions = Instructions()
        constants = []

    def compile(self, node):
        if node.type == StatementType.PROGRAM:
            for stmt in node.statements:
                self.compile(stmt)
        elif node.type == StatementType.EXPRESSION_STATEMENT:
            self.compile(node.expression)
        elif node.type == StatementType.INFIX_EXPRESSION:
            self.compile(node.left)
            self.compile(node.right)
        elif node.type == StatementType.INTEGER:
            value = Ad_Integer_Object(node.value)
        return None

    def bytecode(self):
        return Bytecode()

    def add_constant(self, obj):
        self.constants.append(obj)
        return len(self.constants) - 1

if __name__ == '__main__':
    compiler = Compiler()
