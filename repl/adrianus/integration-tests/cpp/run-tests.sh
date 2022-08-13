# echo "\033[0;36mstarting tests\033[0m" ;

. build-binaries.sh ;

echo "\033[0;36mrunnning tests\033[0m" ; 
python run-tests.py ;
echo "\033[0;32mfinished!\033[0m" ;

echo "\033[0;36mrunnning memory check\033[0m" ; 
python run-memory-checks.py ;
echo "\033[0;32mfinished!\033[0m" ;

. cleanup-tests.sh 