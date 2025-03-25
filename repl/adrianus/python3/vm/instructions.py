class Instructions:
    size = 0

    def __init__(self):
        # a list of bytes
        # List<Byte>
        self.bytes = []

    def get(self, i):
        return self.bytes[i]

    def add(self, ins):
        self.size += 1
        self.bytes.append(ins)

    def __str__(self):
        out = ""
        for instruction in self.bytes:
            out += str(instruction) + "\n"
        return out
