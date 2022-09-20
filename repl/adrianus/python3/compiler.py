from code.instructions import Instructions


class Compiler:
    def __init__(self):
        self.instructions = Instructions()
        constants = []

    def new(self):
        self.instructions = Instructions()
        constants = []

    def compile(self, node):
        if node.type == StatementType.PROGRAM:
            pass
        return None

    def bytecode(self):
        return Bytecode()

if __name__ == '__main__':
    compiler = Compiler()
