import signal
import sys

from environment import new_environment
from bootstrap import load_bootstrap

def signal_ctrl_c_handler(sig, frame):
	sys.exit(0)


class Repl(object):
	def __init__(self, parser=None, program=None, evaluator=None, compiler=None, vm=None):
		self.parser = parser
		self.program = program
		self.evaluator = evaluator
		self.compiler = compiler
		self.vm = vm

	def loop(self):
		signal.signal(signal.SIGINT, signal_ctrl_c_handler)
		print ("Ad interpreter [Python]... v1.1")
		env = new_environment()
		load_bootstrap(self.program, self.parser, self.evaluator, env)
		while True:
			line = input('>> ')
			self.parser.reset(source=line)
			self.program.reset()
			self.parser.build_program_statements(self.program)
			# make this check with a run argument
			if True:
			    self.compiler.reset()
			    self.compiler.compile(self.program)
			    bytecode = self.compiler.get_bytecode()
			    self.vm.load(bytecode)
			    self.vm.run()
			else:
			    self.evaluator.eval(self.program, env)
			#self.program.debug()

	def execute_file(self, source):
		#print (source)
		env = new_environment()
		load_bootstrap(self.program, self.parser, self.evaluator, env)
		self.parser.reset(source=source)
		self.program.reset()
		self.parser.build_program_statements(self.program)
		self.evaluator.eval(self.program, env)
		#self.program.debug()

	def read_line():
        # i need to construct the source input line, one char at a time in order to check for arrow keys
		# unable to do it for now, there's no smart way of differentiating between the arrow keys, not ever readchar from pypi knows how to do it
		# still needs more investigating into char-by-char keyboard reading
		pass
