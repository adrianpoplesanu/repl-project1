import os, sys

p = os.path.abspath('.')
sys.path.insert(1, p)

from objects import Ad_String_Object
#from evaluator import NULLOBJECT # this is parent level, i can't import it yet, maybe move this in a utils top level file

def read_file_content(filepath):
    resource = open(filepath, 'r')
    # added .strip() for having same behaviour with cpp implementation, TODO: check if this is correct in the long run
    return Ad_String_Object(resource.read().strip())

def write_file_content(filepath, content):
    resource = open(filepath, 'w')
    resource.write(content)
    return None

def append_file_content(filepath, content):
    resource = open(filepath, 'a')
    resource.append(content)
    return None
