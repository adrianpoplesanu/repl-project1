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
        failure.append("FAILURE - " + target)
        failure_expected.append(expected_output)
        failure_actual.append(output)
    else:
        success.append(target)

if failure:
    print "There were errors:"
    for f in failure:
        print f
else:
    print 'ALL tests passed!'
    for s in success:
        print s