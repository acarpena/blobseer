#!/bin/bash
# argument file name

temp=_temp
myri=-myri0
nodes=$(cat $1)
for i in $nodes; do
	node=`echo $i | cut -d '.' -f 1`
	echo $node$myri >> $1$temp
done
rm -f $1
mv $1$temp $1



