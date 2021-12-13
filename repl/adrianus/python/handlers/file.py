import os, sys

p = os.path.abspath('.')
sys.path.insert(1, p)

from objects import Ad_String_Object

def read_file_content(filepath):
    resource = open(filepath, 'r')
    return Ad_String_Object(resource.read())