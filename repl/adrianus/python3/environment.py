
class Environment(object):
    def __init__(self):
        self.store = {}
        self.outer = None

    def check(self, key):
        return key in self.store or (self.outer and key in self.outer.store)

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

    def set_local_param(self, key, value):
        # TODO: implement this
        pass

    def add_sibling(self, key, env):
        # TODO: implement this
        pass

    def get_sibling(self, key):
        # TODO: implement this
        return None

    def lookup_only_in_store(self, key):
        # TODO: implement this
        return None

    def lookup_constructor(self):
        # TODO: implement this
        return None

    def print_store(self, level):
        # TODO: implement this
        pass

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
