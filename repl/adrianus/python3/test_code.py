from code import Code
from opcode import Opcode
from opcode import OpcodeEnum

def simple_test():
    code = Code()
    instructions = [code.make(Opcode(OpcodeEnum.OP_ADD), []),
                    code.make(Opcode(OpcodeEnum.OP_CONSTANT), [2]),
                    code.make(Opcode(OpcodeEnum.OP_CONSTANT), [65534])]
    for instruction in instructions:
        for i in instruction:
            code.instructions.instructions.append(i)
    expected = """0000 OpAdd
0001 OpConstant 2
0004 OpConstant 65534
"""
    output = code.to_string()
    if output == expected:
        print ("success")
    else:
        print ("FAILED")
        print ("expected: ", expected)
        print ("actual: ", output)

if __name__ == '__main__':
    simple_test()
