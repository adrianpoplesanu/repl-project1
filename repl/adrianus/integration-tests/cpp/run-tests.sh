echo "starting tests" ;
cd ../../cpp ; 
echo "building project" ;
make clean all ;
echo "runnning tests" ; 
cd ../integration-tests/cpp ;
python run-tests.py ;
echo "DONE - check output" ;
