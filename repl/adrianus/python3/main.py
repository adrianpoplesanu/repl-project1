import sys
from repl import Repl
from parser import Parser
from ast import ASTProgram
from evaluator import Evaluator
from test_parser import test_parser, test_parser2, test_parser3
from compiler import Compiler
from vm import VM

DEBUG = False


def main(args):
    use_vm = False
    filename = ''

    if len(args) > 2:
        print ("wrong number of arguments")
        return
    if len(args) == 2:
        if args[0] == '--vm':
            use_vm = True
        elif args[0] == '--eval':
            use_vm = False
        else:
            print ("first argument one of --vm or --eval")
        filename = args[1]
    if len(args) == 1:
        if args[0] == '--vm':
            use_vm = True
        elif args[0] == '--eval':
            use_vm = False
        else:
            filename = args[0]

    if DEBUG:
        #test_parser()
        #test_parser2()
        test_parser3()
    parser = Parser()
    program = ASTProgram()
    evaluator = Evaluator()
    compiler = Compiler()
    vm = VM()
    repl = Repl(parser=parser, program=program, evaluator=evaluator, compiler=compiler, vm=vm)
    if filename:
        data = open(filename, "r")
        source = data.read()
        repl.execute_file(source=source, use_vm=use_vm)
    else:
        repl.loop(use_vm)


if __name__ == '__main__':
    main(sys.argv[1:])
