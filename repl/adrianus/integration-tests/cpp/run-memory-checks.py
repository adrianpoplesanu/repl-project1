import subprocess

binary_path = "../../cpp/"
binary_excutable = "main"

tests_data = open("test-list.txt", "r")
test_files = tests_data.readlines()

errors_detected = False

for test_file in test_files:
    target = test_file.strip()
    #proc = subprocess.Popen(["leaks", "--atExit", "--", "{0}{1}".format(binary_path, binary_excutable), "{0}{1}".format(binary_path, target)], stdout=subprocess.PIPE)
    proc = subprocess.Popen(["./memory-check-macos.sh", "{0}{1}".format(binary_path, binary_excutable), "{0}{1}".format(binary_path, target)])

    proc.wait()
    proc.communicate()

    result_file = open('mem-results.log')

    if ' 0 leaks for 0 total leaked bytes.' not in result_file.read():
        errors_detected = True
        print '\033[0;31mMEMORY ERROR!\033[0m'
        print test_file
    elif 'MEMORY ERROR!!!' in result_file.read():
        errors_detected = True
        print '\033[0;31mMEMORY ERROR!\033[0m'
        print test_file
    else:
        print 'MEMORY OK!'

    result_file.close()

if not errors_detected:
    print '\033[0;32mALL memory tests passed!\033[0m'

tests_data.close()
