from vm.definition import Definition
from vm.instructions import Instructions
from vm.ad_opcode import Opcode, OpcodeEnum


class Code:
    def __init__(self):
        self.instructions = Instructions()
        self.definitions = {
            OpcodeEnum.OP_CONSTANT: Definition("OpConstant", 1, [2]),
            OpcodeEnum.OP_ADD: Definition("OpAdd", 0, []),
            OpcodeEnum.OP_SUB: Definition("OpSub", 0, []),
            OpcodeEnum.OP_MULTIPLY: Definition("OpMul", 0, []),
            OpcodeEnum.OP_DIVIDE: Definition("OpDivide", 0, []),
            OpcodeEnum.OP_POP: Definition("OpPop", 0, []),
            OpcodeEnum.OP_TRUE: Definition("OpTrue", 0, []),
            OpcodeEnum.OP_FALSE: Definition("OpFalse", 0, []),
            OpcodeEnum.OP_EQUAL: Definition("OpEqual", 0, []),
            OpcodeEnum.OP_NOTEQUAL: Definition("OpNotEqual", 0, []),
            OpcodeEnum.OP_GREATERTHAN: Definition("OpGreaterThan", 0, []),
            OpcodeEnum.OP_GREATERTHAN_EQUAL: Definition("OpGreaterThanEqual", 0, []),
            OpcodeEnum.OP_MINUS: Definition("OpMinus", 0, []),
            OpcodeEnum.OP_BANG: Definition("OpBang", 0, []),
            OpcodeEnum.OP_JUMP_NOT_TRUTHY: Definition("OpJumpNotTruthy", 1, [2]),
            OpcodeEnum.OP_JUMP: Definition("OpJump", 1, [2]),
            OpcodeEnum.OP_NULL: Definition("OpNull", 0, []),
            OpcodeEnum.OP_GET_GLOBAL: Definition("OpGetGlobal", 1, [2]),
            OpcodeEnum.OP_SET_GLOBAL: Definition("OpSetGlobal", 1, [2]),
            OpcodeEnum.OP_ARRAY: Definition("OpArray", 1, [2]),
            OpcodeEnum.OP_HASH: Definition("OpHash", 1, [2]),
            OpcodeEnum.OP_INDEX: Definition("OpIndex", 0, []),
            OpcodeEnum.OP_CALL: Definition("OpCall", 1, [1]),
            OpcodeEnum.OP_RETURN_VALUE: Definition("OpReturnValue", 0, []),
            OpcodeEnum.OP_RETURN: Definition("OpReturn", 0, []),
            OpcodeEnum.OP_GET_LOCAL: Definition("OpGetLocal", 1, [1]),
            OpcodeEnum.OP_SET_LOCAL: Definition("OpSetLocal", 1, [1]),
            OpcodeEnum.OP_GET_BUILTIN: Definition("OpGetBuiltin", 1, [1]),
            OpcodeEnum.OP_CLOSURE: Definition("OpClosure", 2, [2, 1]),
            OpcodeEnum.OP_GET_FREE: Definition("OpGetFree", 1, [1]),
            OpcodeEnum.OP_CURRENT_CLOSURE: Definition("OpCurrentClosure", 0, [])
        }

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
        while i < self.instructions.size:
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
