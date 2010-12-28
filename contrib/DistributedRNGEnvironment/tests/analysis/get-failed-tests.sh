#!/bin/bash

#USAGE: ./get-failed-tests.sh <threshold>

for d in $(ls -d sts-x*)
do
    for i in $(seq 1 13)
    do
        FAILURES=0
	FAILED_TESTS=""
	for stattest in $(ls -d $d/experiments/AlgorithmTesting/*)
	do
	    if [ $stattest == $d/experiments/AlgorithmTesting/FFT ]; then continue; fi
	    if [ $stattest == $d/experiments/AlgorithmTesting/freq ]; then continue; fi
	    DATAS=$stattest/results
	    if [ -e $stattest/data1 ]; then
	        DATAS=$(ls $stattest/data*)
	    fi
	    for res in $DATAS
	    do
		PVALUE=$(cat $res | head -n$i | tail -n1)
	        EQ=$(echo "$PVALUE > 0.001" | bc)
	        if [ $EQ -eq 0 ]; then
	           FAILURES=$(echo "$FAILURES + 1" | bc)
		   FAILED_TESTS="$FAILED_TESTS $stattest"
	        fi
	    done
	done
	echo $FAILURES
	if [ $FAILURES -ge $1 ]; then
	    for t in $FAILED_TESTS; do echo $t; done
	fi
    done
done
