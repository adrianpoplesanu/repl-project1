#!/usr/bin/env bash
echo -e "\033[0;36mcleaning up binaries\033[0m" ;
rm -r examples/ ;
cd ../../cpp ; 
make clean ;
cd - ;
echo -e "\033[0;32mfinished!\033[0m" ; 
