from object_type import ObjectType

class Ad_Object(object):
    type = None

class Ad_Null_Object(Ad_Object):
    type = ObjectType.NULL

class Ad_Integer_Object(Ad_Object):
    type = ObjectType.INTEGER

class Ad_Boolean_Object(Ad_Object):
    type = ObjectType.BOOLEAN

class Ad_String_Object(Ad_Object):
    type = ObjectType.STRING

class Ad_ReturnValue_Object(Ad_Object):
    type = ObjectType.RETURN_VALUE

    def __init__(self, value=None):
        self.value = value

class Ad_Function_Object(Ad_Object):
    type = ObjectType.FUNCTION

class Ad_Error_Object(Ad_Object):
    type = ObjectType.ERROR