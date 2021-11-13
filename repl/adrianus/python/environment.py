
class Environment(object):
    def __init__(self):
        self.store = {}
        self.outer = None

    def check(self, key):
        return key in self.store or key in self.outer

    def get(self, key):
        if key in self.store:
            return self.store.get(key)
        else:
            if self.outer and key in self.outer.store:
                return self.outer.store.get(key)
        return None

    def set(self, key, value):
        if self.outer and key in self.outer.store:
            self.outer.store[key] = value
            return
        self.store[key] = value

    def __str__(self):
        return "[Env] store: " + str(self.store) + " outer: " + str(self.outer)


def new_environment():
    env = Environment()
    return env

def new_enclosed_environment(outer):
    env = Environment()
    env.outer = outer
    return env