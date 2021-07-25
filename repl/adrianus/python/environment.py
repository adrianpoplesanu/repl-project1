
class Environment(object):
    def __init__(self):
        self.store = {}
        self.outer = {}

    def check(self, key):
        return key in self.store or key in self.outer

    def get(self, key):
        if key in self.store:
            return self.store.get(key)
        else:
            if key in self.outer:
                return self.outer.get(key)
        return None

    def set(self, key, value):
        self.store[key] = value


def new_environment():
    env = Environment()
    return env

def new_enclosed_environment(outer):
    env = Environment()
    env.outer = outer.store
    return env