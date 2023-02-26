import subprocess
import platform
import sys

binary_path = "../../cpp/"
binary_excutable = "main"

tests_data = open("test-list.txt", "r")
test_files = tests_data.readlines()

errors_detected = False
os_platform = platform.system()
print (os_platform)

for test_file in test_files:
    target = test_file.strip()
    #proc = subprocess.Popen(["leaks", "--atExit", "--", "{0}{1}".format(binary_path, binary_excutable), "{0}{1}".format(binary_path, target)], stdout=subprocess.PIPE)
    if os_platform.lower() == "darwin":
        proc = subprocess.Popen(["./memory-check-macos.sh", "{0}{1}".format(binary_path, binary_excutable), "{0}{1}".format(binary_path, target)])
    elif os_platform.lower() == "linux":
        proc = subprocess.Popen(["./memory-check-linux.sh", "{0}{1}".format(binary_path, binary_excutable), "{0}{1}".format(binary_path, target)])
    else:
        print ("memory tests pltform issue")
        sys.exit(0)

    proc.wait()
    proc.communicate()

    result_file = open('mem-results.log')
    result_output = result_file.read()

    if os_platform.lower() == "darwin":
        if ' 0 leaks for 0 total leaked bytes.' not in result_output:
            errors_detected = True
            print ('\033[0;31mMEMORY ERROR!\033[0m ### ' + test_file)
            print (result_output)
            print ("===============")
        elif 'MEMORY ERROR!!!' in result_output:
            errors_detected = True
            print ('\033[0;31mMEMORY ERROR!\033[0m @@@ ' + test_file)
            print (result_output)
            print ("===============")
        else:
            print ('MEMORY OK! ' + target)
    elif os_platform.lower() == "linux":
        if ' 0 errors from 0 contexts' not in result_output:
            errors_detected = True
            print ('\033[0;31mMEMORY ERROR!\033[0m ### ' + test_file)
            print (result_output)
            print ("===============")
        elif 'MEMORY ERROR!!!' in result_output:
            errors_detected = True
            print ('\033[0;31mMEMORY ERROR!\033[0m @@@ ' + test_file)
            print (result_output)
            print ("===============")
        else:
            print ('MEMORY OK! ' + target)
    else:
        print ("memory tests verification checks issue")
        sys.exit(0)

    result_file.close()

if not errors_detected:
    print ('\033[0;32mALL memory tests passed!\033[0m')

tests_data.close()
