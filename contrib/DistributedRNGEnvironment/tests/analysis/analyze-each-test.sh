#!/bin/bash

#USAGE: ./analyze-each-test.sh

tests=$(cat tests.txt)

for t in $tests; do
    echo $t
    ./analyze-one-test.sh $t
done
