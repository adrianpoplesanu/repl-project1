from object_type import ObjectType
from hash_utils import HashKey


class Ad_Object(object):
    type = None


class Ad_Null_Object(Ad_Object):
    type = ObjectType.NULL

    def __init__(self):
        """
        nothing to do here
        """
        pass

    def inspect(self):
        return "null"

    def hash_key(self):
        return HashKey(type=type, value=Hash(self.value))


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


class Ad_Float_Object(Ad_Object):
    type = ObjectType.FLOAT

    def __init__(self, value=None):
        """
        @param value: float
        """
        self.value = value

    def inspect(self):
        return str(self.value)

    def hash_key(self):
        return HashMap(type=type, value=hash(self.value))


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

    def inspect(self):
        return self.value.inspect()


class Ad_Break_Object(Ad_Object):
    type = ObjectType.BREAK

    def __init__(self):
        """
        no params, this is a placeholder object that gets evaluated in while and for eval
        """
        pass


class Ad_Continue_Object(Ad_Object):
    type = ObjectType.CONTINUE

    def __init__(self):
        """
        no params, this is a placeholder object that gets evaluated in while and for eval
        """
        pass

class Ad_Function_Object(Ad_Object):
    type = ObjectType.FUNCTION

    def __init__(self, parameters=None, default_params=None, body=None, env=None):
        """
        @param parameters: list of ASTNode
        @:param default_params: list of ASTNode
        @param body: ASTNode
        @param env: Environment
        """
        self.parameters = parameters
        self.default_params = default_params
        self.body = body
        self.env = env

    def inspect(self):
        return "<function at memory address: " + str(hex(id(self))) + ">"


class Ad_String_Object(Ad_Object):
    type = ObjectType.STRING

    def __init__(self, value=None):
        """
        @param value: string
        """
        self.value = value

    def inspect(self):
        return "'" + self.value + "'"

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
        return "ERROR: " + self.message


class Ad_Builtin_Object(Ad_Object):
    # TODO: this needs to have a builtin_function which is actually a class
    type = ObjectType.BUILTIN

    def __init__(self, builtin_function=None, env=None, accepted_parameters_size=None):
        """
        @param builtin_function: reference to a function
        """
        self.builtin_function = builtin_function
        self.env = env
        if accepted_parameters_size:
            self.accepted_parameters_size = accepted_parameters_size
        else:
            self.accepted_parameters_size = []

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
        """
        todo: write this
        """
        self.key = key
        self.value = value


class Ad_Hash_Object(Ad_Object):
    type = ObjectType.HASH

    def __init__(self, pairs=None):
        """
        todo: write this
        """
        self.pairs = pairs

    def inspect(self):
        out = "{"
        out += ', '.join(['{0}: {1}'.format(pair.key.inspect(), pair.value.inspect()) for pair in self.pairs.values()])
        out += "}"
        return out


class Ad_Class_Object(Ad_Object):
    type = ObjectType.CLASS

    def __init__(self, name=None, attributes=None, methods=None, inherit_from=None):
        """
        @param: name
        @param: attributes - after parsing
        @param: methods - after parsing
        @param: inherit_from - classes inherited after parsing
        """
        self.name = name
        self.attributes = attributes
        self.methods = methods
        self.inherit_from = inherit_from

    def inspect(self):
        out = "<class object at memory address: " + str(hex(id(self))) + ">"
        return out


class Ad_Class_Instance(Ad_Object):
    type = ObjectType.INSTANCE

    def __init__(self, name=None, class_object=None, instance_environment=None, inherit_from=None):
        """
        @param: name - AstNode with the AstIdenfitier of the instance name
        @param: class_object - a clone of the Ad_Class_Object against which the logic will be executed
        """
        self.name = name
        self.class_object = class_object
        self.instance_environment = instance_environment
        self.inherit_from = inherit_from

    def inspect(self):
        #out = "ClassInstance " + str(self.name) + " "
        #out += "attributes: " + str(self.class_object.attributes) + " "
        #out += "methods: " + str(self.class_object.methods)
        out = "<class instance at memory address: " + str(hex(id(self))) + ">"
        return out


class Ad_File_Object(Ad_Object):
    type = ObjectType.FILE

    def __init__(self, filename=None, operator=None):
        """
        @param: name - string, the file_path
        @param: operator - string, 'r', 'w'
        """
        self.filename = filename
        self.operator = operator
        # TODO: fix this
        # the next line is wrong, if the operator is w and no write() method is called, the file is created regardlessly
        self.file_descriptor = open(self.filename, self.operator)

    def inspect(self):
        out = "<file object at memory address: " + str(hex(id(self))) + ">"
        return out


class Ad_Socket_Object(Ad_Object):
    type = ObjectType.SOCKET

    def __init__(self, name=None, host=None, port=None, is_active=None,
            is_forever=None, is_client=None, is_server=None, sock=None,
            conn=None, addr=None):
        """
        @param: name - string, the name of the socket
        @param: host - string, the host if the socket
        @param: port - int, the port of the socket
        @param: is_active - boolean, wheter the socket is active or not
        @param: is_forever - boolean, wheter the socket is forever or not
        @param: is_client - boolean, wheter the socket is a client or not
        @param: is_server - boolean, wheter the socket is a server or not
        @param: sock - python socket object
        @param: conn - python connection object
        @param: addr - addr
        """
        self.name = name
        self.host = host
        self.port = port
        self.is_active = is_active
        self.is_forever = is_forever
        self.is_client = is_client
        self.is_server = is_server
        self.sock = sock
        self.conn = conn
        self.addr = addr

    def inspect(self):
        out = "<socket instance at memory address: " + str(hex(id(self))) + ">"
        return out


class Ad_Thread_Object(Ad_Object):
    type = ObjectType.THREAD

    def __init__(self, name=None, callback=None, params=None, thread=None, result=None):
        """
        @param: name - string, the name of the thread
        @param: callback - function that gets executed when thread is running
        @param: params - params for function that gets executed
        @param: thread - thread reference
        @param: result - the return of the callback
        """
        self.name = name
        self.callback = callback
        self.params = params
        self.thread = thread
        self.result = result

    def inspect(self):
        out = "<thread instance at memory address: " + str(hex(id(self))) + ">"
        return out
