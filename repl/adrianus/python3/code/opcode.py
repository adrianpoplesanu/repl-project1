class OpcodeEnum:
    OP_CONSTANT = 0 #"OP_CONSTANT"
    OP_ADD = 1 #"OP_ADD"

class Opcode:
    def __init__(self, byte_code):
        self.byte_code = byte_code
