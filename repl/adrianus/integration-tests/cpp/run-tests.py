import subprocess

binary_path = "../../cpp/"
binary_excutable = "main"
expected_folder = "expected/"

tests_data = open("test-list.txt", "r")
test_files = tests_data.readlines()

success = []
failure = []
failure_expected = []
failure_actual = []

for test_file in test_files:
    target = test_file.strip()
    proc = subprocess.Popen(["{0}{1}".format(binary_path, binary_excutable), "{0}{1}".format(binary_path, target)], stdout=subprocess.PIPE)
    proc.wait()
    output = proc.communicate()[0]
    expected_target = expected_folder + target.split("/")[-1].replace(".ad", ".txt")
    expected_output = open(expected_target, 'r').read()

    if output != expected_output:
        #print "FAILURE - " + target
        #print "EXPECTED:\n" + expected_output
        #print "ACTUAL:\n" + output
        failure.append("FAILURE - " + target)
        failure_expected.append(expected_output)
        failure_actual.append(output)
    else:
        #print "SUCCESS - " + target
        success.append(target)
    #proc = subprocess.Popen(["ls", "-la"])
    #proc = subprocess.Popen(["../../cpp/main", "../../cpp/examples/test01.ad"])
    #output = proc.join()
    #print output

print success
print failure
