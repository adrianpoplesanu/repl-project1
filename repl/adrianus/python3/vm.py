from instructions import Instructions
from opcode import OpcodeEnum
from code import Code
from objects import Ad_Integer_Object

class VM:
    def __init__(self):
        self.constants = []
        self.instructions = None
        self.stack = []
        self.sp = 0
        self.code = Code()

    def load(self, bytecode):
        self.constants = bytecode.constants
        self.instructions = bytecode.instructions
        self.stack = []
        self.sp = 0

    def stack_top(self):
        if len(self.stack):
            return self.stack[-1]
        return None

    def run(self):
        ip = 0
        while ip < self.instructions.size():
            op = self.instructions.get(ip)
            #print (op)
            if op.byte_code == OpcodeEnum.OP_CONSTANT:
                const_index = self.code.read_uint16(self.instructions, ip + 1)
                self.push(self.constants[const_index])
                ip += 2
            elif op.byte_code == OpcodeEnum.OP_ADD:
                right = self.pop()
                left = self.pop()
                result = right.value + left.value
                self.push(Ad_Integer_Object(result))
            ip += 1

    def push(self, obj):
        if self.sp >= 2048:
            raise Exception("stack limit reached")
        self.sp += 1
        self.stack.append(obj)

    def pop(self):
        res = self.stack[self.sp - 1]
        self.sp -= 1
        return res
