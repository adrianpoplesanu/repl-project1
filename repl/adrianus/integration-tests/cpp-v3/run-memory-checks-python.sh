#!/usr/bin/env bash

echo "running memory checks - run tests with platform specific memory checkers and see if there are issues" ;
# 0 leaks for 0 total leaked bytes. trebuie cautat in output-ul memory check-ului

python3 run-memory-checks-python.py ;

rm mem-results.log ;
