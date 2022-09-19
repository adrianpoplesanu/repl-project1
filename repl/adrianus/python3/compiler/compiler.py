import sys
import os
sys.path.append(os.path.abspath('../code'))

from instructions import Instructions

class Compiler:
    def __init__(self):
        self.instructions = Instructions()
        constants = []

    def new(self):
        self.instructions = Instructions()
        constants = []

    def compile(self, node):
        # AstNode node
        return None

    def bytecode(self):
        return Bytecode()

if __name__ == '__main__':
    compiler = Compiler()
