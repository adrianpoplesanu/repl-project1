from vm.instructions import Instructions


class Bytecode:
    def __init__(self):
        self.instructions = Instructions()
        self.constants = [] # List<Ad_Object>

    def __str__(self):
        out = "instructions:\n"
        out += str(self.instructions)
        out += "\nconstants:\n"
        for constant in self.constants:
            out += str(constant.inspect()) + "\n"
        return out
