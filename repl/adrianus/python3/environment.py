
class Environment(object):
    def __init__(self):
        self.store = {}
        self.outer = None
        self.bootstrap = None
        self.siblings = {}
        self.is_instance_environment = False
        self.owning_instance_environment = None

    def check(self, key):
        return key in self.store or (self.outer and self.outer.check(key)) or (self.bootstrap and self.bootstrap.check(key))

    def get(self, key):
        if key in self.store:
            return self.store.get(key)
        if self.outer and self.outer.check(key):
            return self.outer.get(key)
        if self.bootstrap and self.bootstrap.check(key):
            return self.bootstrap.get(key)
        return None

    def set(self, key, value):
        if key in self.store:
            self.store[key] = value
        else:
            if self.outer and self.outer.check(key):
                self.outer.set(key, value)
            else:
                self.store[key] = value
        #if self.outer and key in self.outer.store:
        #    self.outer.store[key] = value
        #    return
        #self.store[key] = value

    def set_local_param(self, key, value):
        self.store[key] = value

    def add_sibling(self, key, env):
        self.siblings[key] = env

    def get_sibling(self, key):
        return self.siblings[key]

    def lookup_only_in_store(self, key):
        if key in self.store:
            return self.store[key]
        return None

    def lookup_constructor(self):
        return self.lookup_only_in_store('constructor')

    def print_store(self, level):
        out = ""
        if level != 0:
            out += "\n"
        k = 0
        offset = ''
        while k < level:
            offset += ' '
            k += 1
        out += offset
        out = "store: {"
        size = len(self.store)
        for i, key in enumerate(self.store.keys()):
            if i < size - 1:
                out += key + ": " + self.store[key].inspect() + ", "
            else:
                out += key + ": " + self.store[key].inspect()
        out += "}\n"
        out += offset
        out += "outer: {"
        if self.outer:
            out += self.outer.print_store(level + 4)
        out += "}"
        return out

    def __str__(self):
        out = "[Env] store: " + str(self.store) + "\n"
        if self.outer:
            out += " outer: " + str(self.outer.store) + "\n"
        return out


def new_environment():
    env = Environment()
    return env

def new_enclosed_environment(outer):
    env = Environment()
    env.outer = outer
    return env
