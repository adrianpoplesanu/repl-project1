import sys
import time

from objects import (Ad_Builtin_Object, Ad_Integer_Object, Ad_Null_Object, Ad_String_Object, Ad_File_Object,
                     Ad_Error_Object, Ad_List_Object, Ad_Hash_Object, Ad_Thread_Object, Ad_Socket_Object)
from object_type import ObjectType
from eval_utils import eval_source, import_source
from thread_utils import sleep_builtin_executor
#from evaluator import NULLOBJECT # TODO: circular import, i need to fix this

def len_builtin(args, env):
    obj = args[0]
    if obj.type == ObjectType.STRING:
        return Ad_Integer_Object(value=len(obj.value))
    if obj.type == ObjectType.LIST:
        return Ad_Integer_Object(value=len(obj.elements))
    return Ad_Integer_Object(value=0)

def first_builtin(args, env):
    obj = args[0]
    if obj.type == ObjectType.STRING:
        return Ad_String_Object(value=obj.value[0])
    if obj.type == ObjectType.LIST:
        return obj.elements[0]
    return Ad_Null_Object()
    #return NULLOBJECT

def print_builtin(args, env):
    for obj in args:
        if obj:
            print(obj.repr(), end='')

def println_builtin(args, env):
    for obj in args:
        if obj:
            print(obj.repr())

def exit_builtin(args, env):
    sys.exit(0)

def ref_count_builtin(args, env):
    print ('ref_count not available in Python implementation')

def type_builtin(args, env):
    target = args[0]
    return Ad_String_Object(target.type)

def append_builtin(args, env):
    list_obj = args[0]
    obj = args[1]
    if list_obj.type == ObjectType.LIST:
        list_obj.elements.append(obj)

def context_builtin(args, env):
    #print (env.print_store(0))
    #result = Ad_String_Object(str(env))
    #return result
    return None

def locals_builtin(args, env):
    result = env.store_to_hash()
    return result

def pop_builtin(args, env):
    result = None
    list_obj = args[0]
    if len(args) == 1:
        result = list_obj.elements.pop()
    elif len(args) == 2:
        idx_obj = args[1]
        result = list_obj.elements[idx_obj.value]
        list_obj.elements.pop(idx_obj.value)
    return result

def remove_builtin(args, env):
    pass

def lower_builtin(args, env):
    pass

def upper_builtin(args, env):
    pass

def iofile_builtin(args, env):
    filename = args[0]
    operator = args[1]
    file_obj = Ad_File_Object(filename=filename.value, operator=operator.value)
    return file_obj

def syssystem_builtin(args, env):
    pass

def iosocket_builtin(args, env):
    socket_obj = Ad_Socket_Object(name=args[0], port=args[1], is_active=args[2],
                                  is_forever=args[3], is_client=args[4], is_server=args[5])
    return socket_obj

def eval_builtin(args, env):
    unescaped_source = args[0]
    eval_source(unescaped_source.value, env)
    return None

def last_builtin(args, env):
    pass

def map_builtin(args, env):
    pass

def input_builtin(args, env):
    pass

def list_builtin(args, env):
    if len(args) == 0:
        listObject = Ad_List_Object()
        listObject.elements = []
        return listObject
    elif len(args) == 1:
        if args[0].type == ObjectType.INTEGER:
            number_elements = args[0].value
            listObject = Ad_List_Object()
            listObject.elements = []
            for i in range(number_elements):
                listObject.elements.append(Ad_Null_Object())
                #listObject.elements.append(NULLOBJECT)
            return listObject
    elif len(args) == 2:
        if args[0].type == ObjectType.INTEGER:
            number_elements = args[0].value
            default_object = args[1]
            list_object = Ad_List_Object()
            list_object.elements = []
            for i in range(number_elements):
                list_object.elements.append(default_object)
            return list_object

def mat_builtin(args, env):
    #return NULLOBJECT
    return Ad_Null_Object()

def hash_builtin(args, env):
    if len(args) == 0:
        hashObject = Ad_Hash_Object(pairs={})
        return hashObject

def hasattr_builtin(args, env):
    pass

def getattr_builtin(args, env):
    target = args[0]
    attr_name = args[1].value
    if target.type == ObjectType.INSTANCE:
        return target.instance_environment.lookup_only_in_store(attr_name)
    elif target.type == ObjectType.CLASS:
        # TODO: should i evaluate the ast node and return an un-bound attribute?
        pass
    return None

def setattr_builtin(args, env):
    if len(args) == 3:
        target = args[0]
        if target.type == ObjectType.INSTANCE:
            name = args[1]
            value = args[2]
            if value.type == ObjectType.FUNCTION:
                value.env = target.instance_environment
                target.instance_environment.store[name.value] = value
            else:
                target.instance_environment.store[name.value] = value
    return None

def getattrs_builtin(args, env):
    if len(args) == 1:
        target = args[0]
        if target.type == ObjectType.INSTANCE:
            result = Ad_List_Object()
            result.elements = []
            for entry in target.instance_environment.populate_getattrs():
                result.elements.append(Ad_String_Object(entry))
            return result
        elif target.type == ObjectType.CLASS:
            result = Ad_List_Object()
            result.elements = []
            for attribute in target.attributes:
                result.elements.append(Ad_String_Object(attribute.expression.name.token_literal()))
            for method in target.methods:
                result.elements.append(Ad_String_Object(method.name.token_literal()))
            return result
        else:
            return Ad_Error_Object("getattrs can only be called on a class instance")
    else:
        return None

def thread_builtin(args, env):
    thread_obj = Ad_Thread_Object()
    return thread_obj

def import_builtin(args, env):
    path = args[0]
    import_source(path.value, env)

def sleep_builtin(args, env):
    sleep_builtin_executor(args[0].value)

builtins_map = {
    "len": Ad_Builtin_Object(builtin_function=len_builtin),
    "exit": Ad_Builtin_Object(builtin_function=exit_builtin),
    "print": Ad_Builtin_Object(builtin_function=print_builtin),
    "println": Ad_Builtin_Object(builtin_function=println_builtin),
    "ref_count": Ad_Builtin_Object(builtin_function=ref_count_builtin),
    "type": Ad_Builtin_Object(builtin_function=type_builtin),
    "__append": Ad_Builtin_Object(builtin_function=append_builtin),
    "__pop": Ad_Builtin_Object(builtin_function=pop_builtin),
    "__remove": Ad_Builtin_Object(builtin_function=remove_builtin),
    "__lower": Ad_Builtin_Object(builtin_function=lower_builtin),
    "__upper": Ad_Builtin_Object(builtin_function=upper_builtin),
    "__context": Ad_Builtin_Object(builtin_function=context_builtin, accepted_parameters_size=[0]),
    "__locals": Ad_Builtin_Object(builtin_function=locals_builtin, accepted_parameters_size=[0]),
    "__iofile": Ad_Builtin_Object(builtin_function=iofile_builtin),
    "__syssystem": Ad_Builtin_Object(builtin_function=syssystem_builtin),
    "__iosocket": Ad_Builtin_Object(builtin_function=iosocket_builtin),
    "eval": Ad_Builtin_Object(builtin_function=eval_builtin),
    "__first": Ad_Builtin_Object(builtin_function=first_builtin),
    "__last": Ad_Builtin_Object(builtin_function=last_builtin),
    "map": Ad_Builtin_Object(builtin_function=map_builtin),
    "input": Ad_Builtin_Object(builtin_function=input_builtin),
    "list": Ad_Builtin_Object(builtin_function=list_builtin),
    "mat": Ad_Builtin_Object(builtin_function=mat_builtin),
    "hash": Ad_Builtin_Object(builtin_function=hash_builtin),
    "hasattr": Ad_Builtin_Object(builtin_function=hasattr_builtin),
    "getattr": Ad_Builtin_Object(builtin_function=getattr_builtin),
    "setattr": Ad_Builtin_Object(builtin_function=setattr_builtin),
    "getattrs": Ad_Builtin_Object(builtin_function=getattrs_builtin),
    "__thread": Ad_Builtin_Object(builtin_function=thread_builtin),
    "import": Ad_Builtin_Object(builtin_function=import_builtin),
    "sleep": Ad_Builtin_Object(builtin_function=sleep_builtin),
    # https://www.w3schools.com/python/python_ref_keywords.asp
	# https://www.w3schools.com/python/python_ref_functions.asp
}
