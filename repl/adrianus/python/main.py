import sys
from repl import Repl
from parser import Parser
from ast import ASTProgram
from evaluator import Evaluator
from test_parser import test_parser, test_parser2

DEBUG = False


def main(args):
    if DEBUG:
        #test_parser()
        test_parser2()
    parser = Parser()
    program = ASTProgram()
    evaluator = Evaluator()
    repl = Repl(parser=parser, program=program, evaluator=evaluator)
    if args:
        filename = open(args[0], "r")
        source = filename.read()
        repl.execute_file(source=source)
    else:
        repl.loop()


if __name__ == '__main__':
    main(sys.argv[1:])
