from parser import Parser
from ast import ASTProgram
import evaluator as Evaluator

def get_unescaped_source(source):
    # TODO: unescape the string
    #print (source)
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
