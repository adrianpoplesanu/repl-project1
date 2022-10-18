from instructions import Instructions
from opcode import OpcodeEnum
from code import Code
from objects import Ad_Integer_Object

class VM:
    def __init__(self):
        self.constants = []
        self.instructions = None
        self.stack = [None] * 2048
        self.sp = 0
        self.code = Code()

    def load(self, bytecode):
        #print (bytecode) # useful print for seeing the actual bytecode
        self.constants = bytecode.constants
        self.instructions = bytecode.instructions
        self.stack = [None] * 2048
        self.sp = 0

    def stack_top(self):
        if self.sp == 0:
            return None
        return self.stack[self.sp - 1]

    def last_popped_stack_element(self):
        if self.sp >= 0:
            return self.stack[self.sp]
        return None

    def run(self):
        ip = 0
        while ip < self.instructions.size():
            op = self.instructions.get(ip)
            if op == OpcodeEnum.OP_CONSTANT:
                const_index = self.code.read_uint16(self.instructions, ip + 1)
                self.push(self.constants[const_index])
                ip += 2
            elif op == OpcodeEnum.OP_ADD:
                right = self.pop()
                left = self.pop()
                result = left.value + right.value
                self.push(Ad_Integer_Object(result))
            elif op == OpcodeEnum.OP_SUB:
                right = self.pop()
                left = self.pop()
                result = left.value - right.value
                self.push(Ad_Integer_Object(result))
            elif op == OpcodeEnum.OP_MUL:
                right = self.pop()
                left = self.pop()
                result = left.value * right.value
                self.push(Ad_Integer_Object(result))
            elif op == OpcodeEnum.OP_DIV:
                right = self.pop()
                left = self.pop()
                result = left.value / right.value
                self.push(Ad_Integer_Object(result))
            elif op == OpcodeEnum.OP_POP:
                self.pop()
            else:
                print ("error: unknow opcode in vm.run()")
            ip += 1

    def push(self, obj):
        if self.sp >= 2048:
            raise Exception("stack limit reached")
        self.stack[self.sp] = obj
        self.sp += 1

    def pop(self):
        res = self.stack[self.sp - 1]
        self.sp -= 1
        return res
