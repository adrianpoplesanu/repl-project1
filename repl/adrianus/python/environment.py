
class Environment(object):
    def __init__(self):
        self.store = {}

    def get(self, key):
        return self.store.get(key)

    def set(self, key, value):
        self.store[key] = value


def new_environment():
    env = Environment()
    return env

def new_enclosed_environment(outer):
    env = Environment()
    return env