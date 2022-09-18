class OpcodeEnum:
    OP_CONSTANT = 0 #"OP_CONSTANT"
    OP_ADD = 1 #"OP_ADD"


class Code:
    def __init__(self):
        self.instructions = []
        self.opcode = None
