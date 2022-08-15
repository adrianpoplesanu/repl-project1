import subprocess

binary_path = '../../cpp/'
binary_excutable = 'main'

target = 'examples/test01.ad'

#proc = subprocess.Popen(["leaks", "--atExit", "--", "{0}{1}".format(binary_path, binary_excutable), "{0}{1}".format(binary_path, target)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
#proc = subprocess.run(["leaks", "--atExit", "--", "{0}{1}".format(binary_path, binary_excutable), "{0}{1}".format(binary_path, target)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
#proc = subprocess.Popen(["leaks", "--atExit", "--", "../../cpp/main", "examples/test01.ad"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
#proc = subprocess.Popen(["leaks", "-atExit", "--", "../../cpp/main", "examples/test01.ad"], stdout=subprocess.PIPE)
#proc = subprocess.Popen("leaks --atExit -- ../../cpp/main examples/test01.ad", shell=True)
#proc = subprocess.Popen(["./memory-check.sh", "../../cpp/main", "examples/test01.ad"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
#proc = subprocess.Popen(["./memory-check.sh", "../../cpp/main", "examples/test01.ad"], stdout=subprocess.PIPE)
proc = subprocess.Popen(["./memory-check.sh", "../../cpp/main", "examples/test01.ad"])
#proc = subprocess.Popen(["ls"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

proc.wait()

#print dir(proc)

#print proc.stdout.read()

#t = os.system("leaks --atExit -- ../../cpp/main examples/test01.ad")
#t = os.popen("leaks --atExit -- ../../cpp/main examples/test01.ad").read()
#print t

proc.communicate()

result_file = open('mem-results.log', 'r')

print '0 leaks for 0 total leaked bytes.' in result_file.read()

result_file.close()


#response = proc.communicate()[0]

#proc.wait()

#print response
#print (proc)