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
    out = compiler.code.to_string()
    expected = """0000 OpConstant 0
0003 OpConstant 1
0006 OpAdd
0007 OpPop
"""
    if out == expected:
        print ('success')
        print (out)
    else:
        print ('FAILED!!!')
        print ('expected: ', expected)
        print ('actual: ', out)

def test_arithmetic_expressions():
    compiler = Compiler()
    parser = Parser()
    parser.reset("1; 2")
    program = ASTProgram()
    program.reset()
    parser.build_program_statements(program)
    compiler.reset()
    compiler.compile(program)
    bytecode = compiler.get_bytecode()
    compiler.code.instructions = bytecode.instructions
    out = compiler.code.to_string()
    expected = """0000 OpConstant 0
0003 OpPop
0004 OpConstant 1
0007 OpPop
"""
    if out == expected:
        print ('success')
        print (out)
    else:
        print ('FAILED!!!')
        print ('expected: ', expected)
        print ('actual: ', out)


if __name__ == '__main__':
    simple_test()
    test_arithmetic_expressions()
