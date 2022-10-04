from definition import Definition
from instructions import Instructions
from opcode import Opcode, OpcodeEnum


class Code:
    def __init__(self):
        self.instructions = Instructions()
        self.definitions = {}
        self.definitions[OpcodeEnum.OP_CONSTANT] = Definition("OpConstant", [2])
        self.definitions[OpcodeEnum.OP_ADD] = Definition("OpAdd", [])

    def lookup(self, op_bytecode):
        if op_bytecode in self.definitions:
            return self.definitions[op_bytecode]
        raise Exception("Opcode with bytecode {0} is undefined".format(op_bytecode))

    def make(self, op, operands):
        definition = self.definitions[op.byte_code]
        instruction_len = 1
        for operand_width in definition.operand_widths:
            instruction_len += operand_width
        instructions = [None] * instruction_len # new byte[instruction_len]
        instructions[0] = op.byte_code

        offset = 1
        i = 0
        for operand in operands:
            width = definition.operand_widths[i]
            i += 1
            if width == 2:
                # binary.BigEndian.PutUint16(instruction[offset:], uint16(o))
                operand_bytes = operand.to_bytes(2, 'big')
                instructions[offset] = operand_bytes[0] # TODO: calculate this
                instructions[offset + 1] = operand_bytes[1] # TODO: calculate this
                pass
            offset += width
        return instructions

    def to_string(self):
        out = ""
        i = 0
        while i < self.instructions.size():
            definition = self.lookup(self.instructions.get(i))
            res = self.read_operands(definition, self.instructions, i + 1)
            operands = res[0]
            read = res[1]
            out += "{0:04} {1}\n".format(i, self.format_instruction(definition, operands))
            i += 1 + read
        return out

    def read_operands(self, definition, instructions, start):
        operands = [None] * len(definition.operand_widths)
        offset = 0

        i = 0
        for width in definition.operand_widths:
            if width == 2:
                operands[i] = self.read_uint16(instructions, start + offset)
            i += 1
            offset += width
        return operands, offset

    def format_instruction(self, definition, operands):
        operand_count = len(definition.operand_widths) + 1
        if operand_count == 1:
            return definition.name
        elif operand_count == 2:
            return "{0} {1}".format(definition.name, operands[0])
        else:
            print("unknowm instruction format")

    def read_uint16(self, instructions, offset):
        return int.from_bytes([instructions.get(offset), instructions.get(offset + 1)], byteorder='big')


if __name__ == '__main__':
    code = Code()
    instructions = [code.make(Opcode(OpcodeEnum.OP_ADD), []),
                    code.make(Opcode(OpcodeEnum.OP_CONSTANT), [2]),
                    code.make(Opcode(OpcodeEnum.OP_CONSTANT), [65534])]
    for instruction in instructions:
        for i in instruction:
            code.instructions.instructions.append(i)
    print (code.to_string())
