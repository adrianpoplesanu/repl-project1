import signal
import sys

from environment import new_environment

def signal_ctrl_c_handler(sig, frame):
	#print ("\nleaving Ad, bye bye!")
	sys.exit(0)


class Repl(object):
	def __init__(self, parser=None, program=None, evaluator=None):
		self.parser = parser
		self.program = program
		self.evaluator = evaluator

	def loop(self):
		signal.signal(signal.SIGINT, signal_ctrl_c_handler)
		print ("Ad interpreter [Python]... v1.0")
		env = new_environment()
		while True:
			line = raw_input('>> ')
			self.parser.reset(source=line)
			self.program.reset()
			self.parser.build_program_statements(self.program)
			self.evaluator.eval(self.program, env)
			#self.program.debug()

	def execute_file(self, source):
		#print (source)
		self.parser.reset(source=source)
		self.program.reset()
		self.parser.build_program_statements(self.program)
		env = new_environment()
		self.evaluator.eval(self.program, env)
		#self.program.debug()