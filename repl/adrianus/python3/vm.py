from instructions import Instructions

class VM:
    def __init__(self):
        self.constants = []
        self.instructions = None
        self.stack = []
        self.sp = 0

    def load(self, bytecode):
        self.constants = bytecode.constants
        self.instructions = bytecode.instructions
        self.stack = []
        self.sp = 0

    def stack_top(self):
        if self.sp == 0:
            return None
        return self.stack[self.sp - 1]
        # asta era mai pythonic asa:
        # if len(self.stack):
        #     return self.stack[-1]
        # return None

    def run(self):
        ip = 0
        while ip < self.instructions.size():
            op = self.instructions.get(ip)
            #print (op)
            if op == 0:
                print ('aaa')
            elif op == 1:
                print ('bbb')
            ip += 1

    def push(self, obj):
        if self.sp >= 2048:
            raise Exception("stack limit reached")

    def pop(self):
        res = self.stack[self.sp - 1]
        self.sp -= 1
        return res
