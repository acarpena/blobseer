#!/bin/bash

#USAGE: ./deploy_tests.sh <clients_file> <nb_process_per_node> <nb_sequences_per_chunk>

for c in $(cat $1)
do
    mkdir $c
    mv $(ls chunks/* | head -n $2) $c/
    oarsh $c $PWD/launch_test.sh $PWD/$c $3 & > $c.stdout 2> $c.stderr
done
