echo 'running tests...'

echo 'copying dependencies\n'
cp -r ../../python3/bootstrap .

python3 run-tests-cpp.py

rm -rf bootstrap
echo '\ncleanup dependencies'

echo 'done'
