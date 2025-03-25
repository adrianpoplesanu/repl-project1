class OpcodeEnum:
    OP_CONSTANT = 0             # "OP_CONSTANT"
    OP_ADD = 1                  # "OP_ADD"
    OP_SUB = 2                  # "OP_SUB"
    OP_MULTIPLY = 3             # "OP_MUL"
    OP_DIVIDE = 4               # "OP_DIVIDE"
    OP_POP = 5                  # "OP_POP"
    OP_TRUE = 6                 # "OP_TRUE"
    OP_FALSE = 7                # "OP_FALSE"
    OP_EQUAL = 8                # "OP_EQUAL"
    OP_NOTEQUAL = 9             # "OP_NOTEQUAL"
    OP_GREATERTHAN = 10         # "OP_GREATERTHAN
    OP_GREATERTHAN_EQUAL = 11   # "OP_GREATERTHAN_EQUAL"
    OP_BANG = 12                # "OP_BANG
    OP_MINUS = 13               # "OP_MINUS"
    OP_JUMP_NOT_TRUTHY = 14     # "OP_JUMP_NOT_TRUTHY"
    OP_JUMP = 15                # "OP_JUMP"
    OP_NULL = 16                # "OP_NULL"
    OP_GET_GLOBAL = 17          # "OP_GET_GLOBAL"
    OP_SET_GLOBAL = 18          # "OP_SET_GLOBAL"
    OP_ARRAY = 19               # "OP_ARRAY"
    OP_HASH = 20                # "OP_HASH"
    OP_INDEX = 21               # "OP_INDEX"
    OP_CALL = 22                # "OP_CALL"
    OP_RETURN_VALUE = 23        # "OP_RETURN_VALUE"
    OP_RETURN = 24              # "OP_RETURN"
    OP_GET_LOCAL = 25           # "OP_GET_LOCAL"
    OP_SET_LOCAL = 26           # "OP_SET_LOCAL"
    OP_GET_BUILTIN = 27         # "OP_GET_BUILTIN"
    OP_CLOSURE = 28             # "OP_CLOSURE"
    OP_GET_FREE = 29            # "OP_GET_FREE"
    OP_CURRENT_CLOSURE = 30     # "OP_CURRENT_CLOSURE"


class Opcode:
    def __init__(self, byte_code):
        self.byte_code = byte_code


class OpConstant(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_CONSTANT)


class OpAdd(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_ADD)


class OpSub(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_SUB)


class OpMul(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_MULTIPLY)


class OpDivide(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_DIVIDE)


class OpPop(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_POP)


class OpTrue(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_TRUE)


class OpFalse(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_FALSE)


class OpEqual(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_EQUAL)


class OpNotEqual(Opcode):
    def __init__(self):
        super().__init__(OpcodeEnum.OP_NOTEQUAL)
