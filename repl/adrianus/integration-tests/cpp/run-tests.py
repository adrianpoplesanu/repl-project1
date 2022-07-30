import subprocess

binary_path = "../../cpp/"
binary_excutable = "main"

tests_data = open("test-list.txt", "r")
test_files = tests_data.readlines()

for test_file in test_files:
    target = test_file.strip()
    #print target
    proc = subprocess.Popen(["{0}{1}".format(binary_path, binary_excutable), "{0}{1}".format(binary_path, target)])
    #print dir(proc)
    proc.wait()
    print "done running"
    #proc = subprocess.Popen(["ls", "-la"])
    #proc = subprocess.Popen(["../../cpp/main", "../../cpp/examples/test01.ad"])
    #output = proc.join()
    #print output
