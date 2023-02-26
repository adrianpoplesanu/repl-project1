#!/bin/sh
valgrind --tool=memcheck --leak-check=full $1 $2 > mem-results.log 2>&1
