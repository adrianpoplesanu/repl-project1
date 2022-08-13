import subprocess

binary_path = "../../cpp/"
binary_excutable = "main"

tests_data = open("test-list.txt", "r")
test_files = tests_data.readlines()

for test_file in test_files:
    target = test_file.strip()
    print 'running for ' + test_file