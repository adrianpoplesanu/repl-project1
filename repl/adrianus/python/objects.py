from object_type import ObjectType
from hash_utils import HashKey


class Ad_Object(object):
    type = None


class Ad_Null_Object(Ad_Object):
    type = ObjectType.NULL


class Ad_Integer_Object(Ad_Object):
    type = ObjectType.INTEGER

    def __init__(self, value=None):
        """
        @param value: int
        """
        self.value = value

    def inspect(self):
        return str(self.value)

    def hash_key(self):
        return HashKey(type=type, value=hash(self.value))


class Ad_Boolean_Object(Ad_Object):
    type = ObjectType.BOOLEAN

    def __init__(self, value=None):
        """
        @param value: bool
        """
        self.value = value

    def inspect(self):
        if self.value:
            return 'true' # this might be tied with the keyword mapping, would make it easier to change in the future
        else:
            return 'false' # this might be ties with the keyword mapping, would make it easier to change in the future

    def hash_key(self):
        return HashKey(type=type, value=hash(self.value))


class Ad_ReturnValue_Object(Ad_Object):
    type = ObjectType.RETURN_VALUE

    def __init__(self, value=None):
        """
        @param value: Ad_Object subclass
        """
        self.value = value


class Ad_Function_Object(Ad_Object):
    type = ObjectType.FUNCTION

    def __init__(self, parameters=None, body=None, env=None):
        """
        @param parameters: list of ASTNode
        @param body: ASTNode
        @param env: Environment
        """
        self.parameters = parameters
        self.body = body
        self.env = env

    def inspect(self):
        return "function at memory address: " + str(hex(id(self)))


class Ad_String_Object(Ad_Object):
    type = ObjectType.STRING

    def __init__(self, value=None):
        """
        @param value: string
        """
        self.value = value

    def inspect(self):
        return self.value

    def hash_key(self):
        return HashKey(type=type, value=hash(self.value))


class Ad_Error_Object(Ad_Object):
    type = ObjectType.ERROR

    def __init__(self, message=None):
        """
        @param message: string
        """
        self.message = message

    def inspect(self):
        return self.message


class Ad_Builtin_Object(Ad_Object):
    type = ObjectType.BUILTIN

    def __init__(self, builtin_function=None):
        """
        @param builtin_function: reference to a function
        """
        self.builtin_function = builtin_function

    def inspect(self):
        return str(self.builtin_function)


class Ad_List_Object(Ad_Object):
    type = ObjectType.LIST

    def __init__(self, elements=None):
        """
        @param elements: list of Ad_Objects
        """
        self.elements = elements

    def inspect(self):
        out = "["
        out += ', '.join([element.inspect() for element in self.elements])
        out += "]"
        return out


class Hash_Pair(object):
    def __init__(self, key=None, value=None):
        self.key = key
        self.value = value


class Ad_Hash_Object(Ad_Object):
    type = ObjectType.HASH

    def __init__(self, pairs=None):
        self.pairs = pairs

    def inspect(self):
        out = "{"
        out += ', '.join(['{0}: {1}'.format(pair.key.inspect(), pair.value.inspect()) for pair in self.pairs.values()])
        out += "}"
        return out