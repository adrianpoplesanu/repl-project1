from object_type import ObjectType


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


class Ad_String_Object(Ad_Object):
    type = ObjectType.STRING

    def __init__(self, value=None):
        """
        @param value: string
        """
        self.value = value

    def inspect(self):
        return None


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


class Ad_String_Object(Ad_Object):
    type = ObjectType.STRING

    def __init__(self, value=None):
        self.value = value

    def inspect(self):
        return self.value


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