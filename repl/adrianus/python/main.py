import sys
from repl import Repl
from lexer import Lexer
from parser import Parser
from ast import Program


def main(args):
	lexer = Lexer()
	parser = Parser()
	program = Program()
	repl = Repl(lexer=lexer, parser=parser, program=program)
	if args:
		filename = open(args[0], "r")
		source = filename.read()
		repl.execute_file(source=source)
	else:
		repl.loop()


if __name__ == '__main__':
	main(sys.argv[1:])