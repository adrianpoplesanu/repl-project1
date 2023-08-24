#!/usr/bin/env bash
# echo "\033[0;36mstarting tests\033[0m" ;

cp -r ../../cpp/bootstrap .
. build-binaries.sh ;

echo -e "\033[0;36mrunnning tests\033[0m" ;
python3 run-tests-java.py ;
echo -e "\033[0;32mfinished!\033[0m" ;

echo -e "\033[0;36mrunnning memory check\033[0m" ;
#python run-memory-checks.py ;
. run-memory-checks-java.sh
echo -e "\033[0;32mfinished!\033[0m" ;

rm -r bootstrap/ ;
. cleanup-tests.sh
