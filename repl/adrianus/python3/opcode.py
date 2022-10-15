class OpcodeEnum:
    OP_CONSTANT = 0 #"OP_CONSTANT"
    OP_ADD = 1 #"OP_ADD"
    OP_POP = 2 #"OP_POP"
    OP_SUB = 3 #"OP_SUB"
    OP_MUL = 4 #"OP_MUL"
    OP_DIV = 5 #"OP_DIV"


class Opcode:
    def __init__(self, byte_code):
        self.byte_code = byte_code


class OpAdd(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_ADD)


class OpConstant(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_CONSTANT)


class OpPop(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_POP)


class OpSub(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_SUB)


class OpMul(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_MUL)


class OpDiv(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_DIV)
