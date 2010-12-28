#!/bin/bash

#USAGE: ./launch_test.sh <work_dir> <nb_sequences_per_node>

cd $1
for dump in $(ls x*)
do
    tar xzf ../sts-bgirault.tar.gz
    mv sts-2.0b sts-$dump
    cd sts-$dump
    ./run-test.sh $1/$dump $2 & > $dump.stdout 2> $dump.stderr
    cd ..
done
