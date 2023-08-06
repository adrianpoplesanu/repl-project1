from parser import Parser
from ast import ASTProgram
import evaluator as Evaluator
import re

def get_unescaped_source(source):
    source = re.sub(r"\\\"", "\"", source)
    source = re.sub(r"\\\'", "\'", source)
    return source

def eval_source(source, env):
    parser = Parser()
    program = ASTProgram()
    evaluator = Evaluator.Evaluator()

    source = get_unescaped_source(source)
    parser.reset(source=source)
    program.reset()
    parser.build_program_statements(program)
    result = evaluator.eval(program, env)

def import_source(path, env):
    try:
        source = open(path, 'r').read()
        eval_source(source, env)
    except FileNotFoundError as err:
        pass
