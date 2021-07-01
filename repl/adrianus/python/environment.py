
class Environment(object):
    def __init__(self):
        self.store = {}

    def get(self, key):
        return None

    def set(self, key, value):
        pass


def new_environment():
    env = Environment()
    return env

def new_enclosed_environment(outer):
    env = Environment()
    return env