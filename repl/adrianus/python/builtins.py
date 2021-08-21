import sys

from objects import Ad_Builtin_Object, Ad_Integer_Object, Ad_Null_Object, Ad_String_Object
from object_type import ObjectType

def len_builtin(args):
    obj = args[0]
    if obj.type == ObjectType.STRING:
        return Ad_Integer_Object(value=len(obj.value))
    if obj.type == ObjectType.LIST:
        return Ad_Integer_Object(value=len(obj.elements))
    return Ad_Integer_Object(value=0)

def first_builtin(args):
    obj = args[0]
    if obj.type == ObjectType.STRING:
        return Ad_String_Object(value=obj.value[0])
    if obj.type == ObjectType.LIST:
        return obj.elements[0]
    return Ad_Null_Object

def print_builtin(args):
    for obj in args:
        print obj.inspect()

def exit_builtin(args):
    sys.exit(0)

def ref_count_builtin(args):
    print 'ref_count not available in Python implementation'

def type_builtin(args):
    return Ad_String_Object("type 'aaa'")

builtins_map = {
    "len": Ad_Builtin_Object(len_builtin),
    "first": Ad_Builtin_Object(first_builtin),
    "print": Ad_Builtin_Object(print_builtin),
    "exit": Ad_Builtin_Object(exit_builtin),
    "ref_count": Ad_Builtin_Object(ref_count_builtin),
    "type": Ad_Builtin_Object(type_builtin)
}