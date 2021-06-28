import sys
from repl import Repl
from parser import Parser
from ast import ASTProgram
from test_parser import test_parser

DEBUG = True


def main(args):
    if DEBUG:
        test_parser()
    parser = Parser()
    program = ASTProgram()
    repl = Repl(parser=parser, program=program)
    if args:
        filename = open(args[0], "r")
        source = filename.read()
        repl.execute_file(source=source)
    else:
        repl.loop()


if __name__ == '__main__':
    main(sys.argv[1:])
