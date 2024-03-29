class Instructions:
    def __init__(self):
        # a list of bytes
        # List<Byte>
        self.instructions = []

    def size(self):
        return len(self.instructions)

    def get(self, i):
        return self.instructions[i]

    def add(self, ins):
        self.instructions.append(ins)

    def __str__(self):
        out = ""
        for instruction in self.instructions:
            out += str(instruction) + "\n"
        return out
