class Definition:
    def __init__(self, name: str = None, size: int = None, operand_widths: list = None):
        """
        :param name: name of the definition, ex: OpAdd, OpSub, OpMultiply, OpJump, OpGetGlobal, etc...
        :param size: number of operands
        :param operand_widths: the width of the opcode for each operand, 0 opcode with no operand, ex: 0, 1, 2, etc...
        """
        self.name = name
        self.size = size
        self.operand_widths = operand_widths
