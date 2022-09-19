import sys
import os
sys.path.append(os.path.abspath('../code'))

class Bytecode:
    def __init__(self):
        self.instructions = Instructions()
        self.constants = []
