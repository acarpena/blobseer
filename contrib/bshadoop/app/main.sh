#!/bin/bash
$SCRIPTS_HOME/check_env.sh
if [ $? -ne 0 ]
then 
    echo "Environment not configured properly. Check env.sh and source it."
    exit 1
fi

#$SCRIPTS_HOME/auto-config.sh

SLAVES=`cat $HADOOP_HOME/conf/slaves | wc -l`

#count=$SLAVES

count=1

CLIENT=`cat $HADOOP_HOME/conf/client.txt`

#while [ $count -gt 0 ]
while [ $count -lt $SLAVES ]
do
  	echo "count: $count"
	echo "Generating text..."
	$SCRIPTS_HOME/generate_text.sh $count 1 hdfs
	echo "Done"
	echo "Getting file..."
	oarsh $CLIENT "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/hadoop fs -get $HFILE $FILE"
	echo "Done"
	
	$SCRIPTS_HOME/dist_grep.sh hdfs $count
	sleep 5

	$SCRIPTS_HOME/dist_grep.sh blobseer $count

	rm -rf $FILE
	count=`expr $count + 10`
done

