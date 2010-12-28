#!/bin/bash

#USAGE: ./get_nb_errors.sh <result_file> <nb_max_errors>

for i in $(seq 0 $2)
do
    echo "$i: #$(grep -e "^$i\$" $1 | wc -l)"
done
