echo "building project"

./build_project.sh

cd ../../integration-tests/java ;
cp -r ../../java/ad/bootstrap . ;
cp -r ../../java/ad/examples . ;

python3 run-tests.py

./cleanup.sh

echo "done"
