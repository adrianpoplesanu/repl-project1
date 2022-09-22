from instructions import Instructions

class VM:
    def __init__(self):
        self.constants = []
        self.instructions = None
        self.stack = []
        self.sp = 0

    def load(self, bytecode):
        self.constants = bytecode.constants
        self.instructions = bytecode.Instructions()
        self.stack = []
        self.sp = 0
