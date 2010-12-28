#!/bin/bash

#USAGE: ./analyze-one-test.sh <result_file>
#Note: result_file is a file with its parent directory (e.g. ApproximateEntropy/results)

FAILURES=0
for d in $(ls -d sts-x*)
do
    res=$d/experiments/AlgorithmTesting/$1
    for i in $(seq 1 13)
    do
        PVALUE=$(cat $res | head -n$i | tail -n1)
	EQ=$(echo "$PVALUE > 0.001" | bc)
	if [ $EQ -eq 0 ]; then
	    FAILURES=$(echo "$FAILURES + 1" | bc)
	fi
    done
done
echo $FAILURES
