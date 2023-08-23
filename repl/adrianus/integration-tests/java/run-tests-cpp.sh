echo "building project"

./build_project.sh

cd ../../integration-tests/java ;
cp -r ../../java/ad/bootstrap . ;
cp -r ../../cpp/examples . ;

python3 run-tests-cpp.py

./cleanup.sh

echo "done"
