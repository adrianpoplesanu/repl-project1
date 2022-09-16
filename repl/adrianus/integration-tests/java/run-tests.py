import subprocess
import re

proc = subprocess.Popen(["java", "-jar", "../../java/ad/ad-1.0-SNAPSHOT.jar", "examples/test01.ad"], stdout=subprocess.PIPE)
proc.wait()
output = proc.communicate()[0]
print(output.decode("utf-8"))
