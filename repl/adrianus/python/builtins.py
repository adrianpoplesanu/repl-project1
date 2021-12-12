import sys

from objects import Ad_Builtin_Object, Ad_Integer_Object, Ad_Null_Object, Ad_String_Object
from object_type import ObjectType

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
    return Ad_Null_Object

def print_builtin(args, env):
    for obj in args:
        print obj.inspect()

def exit_builtin(args, env):
    sys.exit(0)

def ref_count_builtin(args, env):
    print 'ref_count not available in Python implementation'

def type_builtin(args, env):
    target = args[0]
    return Ad_String_Object(target.type)

def append_builtin(args, env):
    list_obj = args[0]
    obj = args[1]
    if list_obj.type == ObjectType.LIST:
        list_obj.elements.append(obj)

def context_builtin(args, env):
    print env

def pop_builtin(args, env):
    pass

def remove_builtin(args, env):
    pass

def lower_builtin(args, env):
    pass

def upper_builtin(args, env):
    pass

def file_builtin(args, env):
    pass

def system_builtin(args, env):
    pass

def socket_builtin(args, env):
    pass

def eval_builtin(args, env):
    pass

def first_builtin(args, env):
    pass

def input_builtin(args, env):
    pass

builtins_map = {
    "len": Ad_Builtin_Object(len_builtin),
    "exit": Ad_Builtin_Object(exit_builtin),
    "print": Ad_Builtin_Object(print_builtin),
    "ref_count": Ad_Builtin_Object(ref_count_builtin),
    "type": Ad_Builtin_Object(type_builtin),
    "append": Ad_Builtin_Object(append_builtin),
    "pop": Ad_Builtin_Object(pop_builtin),
    "remove": Ad_Builtin_Object(remove_builtin),
    "lower": Ad_Builtin_Object(lower_builtin),
    "upper": Ad_Builtin_Object(upper_builtin),
    "context": Ad_Builtin_Object(context_builtin),
    "file": Ad_Builtin_Object(file_builtin),
    "system": Ad_Builtin_Object(system_builtin),
    "socket": Ad_Builtin_Object(socket_builtin),
    "eval": Ad_Builtin_Object(eval_builtin),
    "first": Ad_Builtin_Object(first_builtin),
    "input": Ad_Builtin_Object(input_builtin)
}