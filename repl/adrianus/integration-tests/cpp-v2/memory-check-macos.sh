#!/bin/sh
leaks --atExit -- $1 $2 > mem-results.log 2>&1
