import subprocess
import re

binary_path = "../../cpp/"
binary_excutable = "main"
tests_path = "../../java/ad/"
expected_folder = "../java/expected/"

tests_data = open("../java/test-list.txt", "r")
test_files = tests_data.readlines()

success = []
failure = []
failure_expected = []
failure_actual = []

for test_file in test_files:
    target = test_file.strip()
    print ('running test: ' + target)
    proc = subprocess.Popen(["{0}{1}".format(binary_path, binary_excutable), "{0}{1}".format(tests_path, target)], stdout=subprocess.PIPE)
    proc.wait()
    output = proc.communicate()[0]
    expected_target = expected_folder + target.split("/")[-1].replace(".ad", ".txt")
    expected_output = open(expected_target, 'r').read()

    #ok = re.search(expected_output, output)
    ok = re.fullmatch(expected_output, output.decode('utf-8'))
    #if output != expected_output:
    if not ok:
        failure.append("FAILURE - " + target)
        failure_expected.append(expected_output)
        failure_actual.append(output)
    else:
        success.append(target)

if failure:
    print ("\033[0;31mThere were errors:")
    for i, f in enumerate(failure):
        print ("\033[0;31m" + f + "\033[0m")
        print ("\033[0;33mEXPECTED:\033[0m\n" + failure_expected[i])
        print ("\033[0;33mACTUAL:\033[0m\n" + failure_actual[i].decode('utf-8'))
else:
    print ('\033[0;32mALL tests passed!\033[0m')
    for s in success:
        print (s)
