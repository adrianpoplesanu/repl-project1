#!/usr/bin/env bash
echo -e "\033[0;36mbuilding binaries\033[0m" ;
cd ../../cpp-v3 ; 
make clean all ;
cd ../integration-tests/cpp-v3 ;
cp -r ../../cpp-v3/examples . ;
echo -e "\033[0;32mfinished!\033[0m"
