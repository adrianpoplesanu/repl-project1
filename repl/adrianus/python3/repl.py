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

    def loop(self, use_vm=None):
        signal.signal(signal.SIGINT, signal_ctrl_c_handler)
        print ("Ad interpreter [Python]... v3.0") # v2 - classes, v3 - bytecode
        env = new_environment()
        bootstrap_env = load_bootstrap(self.program, self.parser, self.evaluator)
        env.bootstrap = bootstrap_env
        while True:
            line = input('>> ')
            self.parser.reset(source=line)
            self.program.reset()
            self.parser.build_program_statements(self.program)
            # make this check with a run argument
            if use_vm:
                self.compiler.reset()
                self.compiler.compile(self.program)
                bytecode = self.compiler.get_bytecode()
                self.vm.load(bytecode)
                self.vm.run()
                # TODO: remove the next 3 lines
                result = self.vm.last_popped_stack_element()
                if result:
                    print (result.inspect())
            else:
                self.evaluator.eval(self.program, env)
            #self.program.debug()

    def execute_file(self, source, use_vm=None):
        #print (source)
        env = new_environment()
        bootstrap_env = load_bootstrap(self.program, self.parser, self.evaluator)
        env.bootstrap = bootstrap_env
        self.parser.reset(source=source)
        self.program.reset()
        self.parser.build_program_statements(self.program)
        if use_vm:
            self.compiler.reset()
            self.compiler.compile(self.program)
            bytecode = self.compiler.get_bytecode()
            self.vm.load(bytecode)
            self.vm.run()
        else:
            self.evaluator.eval(self.program, env)
        #self.program.debug()

    def read_line():
        # i need to construct the source input line, one char at a time in order to check for arrow keys
        # unable to do it for now, there's no smart way of differentiating between the arrow keys, not ever readchar from pypi knows how to do it
        # still needs more investigating into char-by-char keyboard reading
        pass
