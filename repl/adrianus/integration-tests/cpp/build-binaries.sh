#!/usr/bin/env bash
echo -e "\033[0;36mbuilding binaries\033[0m" ;
cd ../../cpp ; 
make clean all ;
cd ../integration-tests/cpp ;
cp -r ../../cpp/examples . ;
echo -e "\033[0;32mfinished!\033[0m"
