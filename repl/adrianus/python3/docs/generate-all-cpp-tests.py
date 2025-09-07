filename = open("run-cpp-tests.sh", "w")
i = 1

excluded = [45, 66, 141, 142, 143, 151, 152, 155, 170, 175, 177, 180, 195, 196, 197, 198, 199]

for i in range(200):
    number = str(i)
    if int(number) in excluded:
        continue
    if int(number) < 10:
        number = '00' + number
    elif int(number) < 100:
        number = '0' + number
    filename.write('echo "running test{0}.ad" ; python3 main.py ../cpp/examples/test{0}.ad ;\n'.format(number))

filename.close()
