from compiler import Compiler
from parser import Parser
from ast import ASTProgram

def simple_test():
    compiler = Compiler()
    parser = Parser()
    parser.reset("1+2")
    program = ASTProgram()
    program.reset()
    parser.build_program_statements(program)
    compiler.reset()
    compiler.compile(program)
    bytecode = compiler.get_bytecode()
    compiler.code.instructions = bytecode.instructions
    print (compiler.code.to_string())

if __name__ == '__main__':
    simple_test()
