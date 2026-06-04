import re

expected_output = """store: {Elev: class object at memory address (.*)}
outer: {}
58
"""

actual = """store: {Elev: class object at memory address 0x146f08f10}
outer: {}
58
"""

ok = re.fullmatch(expected_output, actual)

print (ok)
#print (ok.groups)
#print (dir(ok))
