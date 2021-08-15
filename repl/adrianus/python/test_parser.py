from repl import Repl
from parser import Parser
from ast import ASTProgram
from utils import print_ast_nodes


def test_parser():
    source = "let a = 1; let add = fn(a,b) { 1+2+3; } ; werty; ; 1 + 2 - 3 * 4; add(a, 747); if (3 > 1) { 1001; } else { 1002; } let a = true; let b = false;"
    parser = Parser()
    program = ASTProgram()
    parser.reset(source=source)
    program.reset()
    parser.build_program_statements(program)
    print_ast_nodes(program, 0)

def test_parser2():
    source = "let a = 1;"
    parser = Parser()
    program = ASTProgram()
    parser.reset(source=source)
    program.reset()
    parser.build_program_statements(program)
    print_ast_nodes(program, 0)
