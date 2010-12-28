#!/bin/bash

#USAGE: ./get-successive-non-null-number-of-failures.sh <result_file>

CUR_NB_FAILURES=0
for i in $(cat $1)
do
    if [ $i == "0" ]; then
        echo $CUR_NB_FAILURES
        CUR_NB_FAILURES=0
    else
        CUR_NB_FAILURES=$(($CUR_NB_FAILURES + 1))
    fi
done
echo $CUR_NB_FAILURES
