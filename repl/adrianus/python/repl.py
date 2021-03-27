import signal
import sys


def signal_ctrl_c_handler(sig, frame):
	print ("\nleaving \033[1;32;48mAdrianus\033[1;37;0m, bye bye!")
	sys.exit(0)


class Repl(object):
	def __init__(self, lexer=None, parser=None, program=None):
		self.repl = lexer
		self.parser = parser
		self.program = program

	def loop(self):
		signal.signal(signal.SIGINT, signal_ctrl_c_handler)
		print ("welcome to \033[1;32;48mAdrianus\033[1;37;0m programming language")
		while True:
			line = input('>> ')
			self.parser.reset(source=line)
			self.program.reset()
			self.parser.build_program_statements(self.program)
			self.program.debug()

	def execute_file(self, source):
		print (source)