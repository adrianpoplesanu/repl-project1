from definition import Definition
from opcode import Opcode

class OpcodeEnum:
    OP_CONSTANT = 0 #"OP_CONSTANT"
    OP_ADD = 1 #"OP_ADD"


class Code:
    def __init__(self):
        self.instructions = []
        self.opcode = None

        self.definitions = {}
        self.definitions[OpcodeEnum.OP_CONSTANT] = Definition("OpConstant", [2])
        self.definitions[OpcodeEnum.OP_ADD] = Definition("OpAdd", [])

    def lookup(self, op):
        if op in self.definitions:
            return self.definitions[op]
        raise Exception("Opcode {0} is undefined".format(op))

    def make(self, op, *args):
        definition = self.definitions[op.byte_code]
        instruction_len = 1
        for operand_width in definition.operand_widths:
            instruction_len += operand_width
        instruction = [None] * instruction_len # new byte[instruction_len]
        instruction[0] = op

        offset = 1
        i = 0
        for operand in args:
            width = definition.operand_widths[i]
            i += 1
            if width == 2:
                # binary.BigEndian.PutUint16(instruction[offset:], uint16(o))
                operand_bytes = operand.to_bytes(2, 'big')
                instruction[offset] = operand_bytes[0] # TODO: calculate this
                instruction[offset + 1] = operand_bytes[1] # TODO: calculate this
                pass
            offset += width
        return instruction


if __name__ == '__main__':
    code = Code()
    instructions = [code.make(Opcode(OpcodeEnum.OP_ADD)),
                    code.make(Opcode(OpcodeEnum.OP_CONSTANT), 2),
                    code.make(Opcode(OpcodeEnum.OP_CONSTANT), 65534)]
    print(instructions)
