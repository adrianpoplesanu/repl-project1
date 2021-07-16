import sys

from objects import Ad_Builtin_Object, Ad_Integer_Object
from object_type import ObjectType

def len_builtin(args):
    obj = args[0]
    if obj.type == ObjectType.STRING:
        return Ad_Integer_Object(value=len(obj.value))
    return Ad_Integer_Object(value=0)

def exit_builtin(args):
    sys.exit(0)

builtins_map = {
    "len": Ad_Builtin_Object(len_builtin),
    "exit": Ad_Builtin_Object(exit_builtin)
}