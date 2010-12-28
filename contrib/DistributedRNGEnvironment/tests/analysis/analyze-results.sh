#!/bin/bash

#USAGE: ./analyze-results.sh

for d in $(ls -d sts-x*)
do
    for i in $(seq 1 13)
    do
        FAILURES=0
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
	        fi
	    done
	done
	echo $FAILURES
    done
done
