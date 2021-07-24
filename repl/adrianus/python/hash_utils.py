class HashKey(object):
    def __init__(self, type=None, value=None):
        """
        @param: type - ObjectType
        @param: value - hash int, ex: 3943517107174772026
        """
        self.type = type
        self.value = value