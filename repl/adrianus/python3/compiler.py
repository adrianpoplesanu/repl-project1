from instructions import Instructions
from bytecode import Bytecode
from objects import Ad_Integer_Object
from parser import Parser
from code import Code
from ast import ASTProgram
from ast import StatementType
from opcode import OpConstant, OpAdd


class Compiler:
    def __init__(self):
        self.instructions = Instructions()
        self.code = Code()
        self.constants = []

    def new(self):
        self.instructions = Instructions()
        constants = []

    def init(self):
        self.instructions = Instructions()
        self.bytecode = Bytecode()

    def compile(self, node):
        if node.type == StatementType.PROGRAM:
            for stmt in node.statements:
                self.compile(stmt)
        elif node.type == StatementType.EXPRESSION_STATEMENT:
            self.compile(node.expression)
        elif node.type == StatementType.INFIX_EXPRESSION:
            self.compile(node.left)
            self.compile(node.right)
            if node.operator == '+':
                self.emit(OpAdd(), [])
        elif node.type == StatementType.INTEGER:
            integer = Ad_Integer_Object(node.value)
            self.emit(OpConstant(), [self.add_constant(integer)])
        return None

    def bytecode(self):
        return Bytecode()

    def add_constant(self, obj):
        self.constants.append(obj)
        return len(self.constants) - 1

    def emit(self, op, operands):
        ins = self.code.make(op, operands)
        pos = self.add_instruction(ins)
        return pos

    def add_instruction(self, instructions):
        pos_new_instruction = self.instructions.size()
        for instruction in instructions:
            self.instructions.add(instruction)
        return pos_new_instruction

    def get_bytecode(self):
        self.bytecode.instructions = self.instructions
        self.bytecode.constants = self.constants
        return self.bytecode

if __name__ == '__main__':
    compiler = Compiler()
    parser = Parser()
    parser.reset("1+2")
    program = ASTProgram()
    program.reset()
    parser.build_program_statements(program)
    compiler.init()
    compiler.compile(program)
    bytecode = compiler.get_bytecode()
    compiler.code.instructions = bytecode.instructions
    print (compiler.code.print())
