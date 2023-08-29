echo "building project"

./build_project.sh

cd ../../integration-tests/java ;
cp -r ../../java/ad/bootstrap . ;
cp -r ../../python3/examples . ;

python3 run-tests-python.py

./cleanup.sh

echo "done"
