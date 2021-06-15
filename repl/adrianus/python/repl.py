import signal
import sys


def signal_ctrl_c_handler(sig, frame):
	#print ("\nleaving Ad, bye bye!")
	sys.exit(0)


class Repl(object):
	def __init__(self, parser=None, program=None):
		self.parser = parser
		self.program = program

	def loop(self):
		signal.signal(signal.SIGINT, signal_ctrl_c_handler)
		print ("Ad interpreter [Python]... v1.0")
		while True:
			line = raw_input('>> ')
			self.parser.reset(source=line)
			self.program.reset()
			self.parser.build_program_statements(self.program)
			self.program.debug()

	def execute_file(self, source):
		print (source)