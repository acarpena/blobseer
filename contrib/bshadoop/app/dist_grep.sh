#!/bin/bash

# arguments: what to deploy (hdfs / blobseer), no of clients

$SCRIPTS_HOME/check_env.sh
if [ $? -ne 0 ]
then
    echo "Environment not configured properly. Check env.sh and source it."
    exit 1
fi

EXAMPLE_JAR=$HADOOP_HOME/build/hadoop-$HADOOP_VERSION-examples.jar

JOBTRACKER=`cat $HADOOP_HOME/conf/jobtracker.txt`
CLIENT=`cat $HADOOP_HOME/conf/client.txt`
oarsh $JOBTRACKER "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/stop-mapred.sh"
oarsh $JOBTRACKER "killall java"
$SCRIPTS_HOME/$1-deploy.sh

$HADOOP_HOME/bin/hadoop fs -mkdir /tmp/hadoop-dmoise/mapred/system
$HADOOP_HOME/bin/hadoop fs -mkdir /tmp/hadoop-dmoise/mapred/local/jobTracker

oarsh $JOBTRACKER "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/start-mapred.sh"
echo "Deployment done"
sleep 10

mkdir -p $HADOOP_HOME/benchmark/apps/grep_logs

oarsh $CLIENT "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/hadoop fs -put $FILE $HFILE"

$HADOOP_HOME/bin/hadoop fs -rmr $HFILE/_logs
$HADOOP_HOME/bin/hadoop fs -ls $HFILE

sleep 5

echo "Starting grep..."
oarsh $CLIENT "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/hadoop jar $EXAMPLE_JAR grep $HFILE output 'a' &> $HADOOP_HOME/benchmark/apps/grep_logs/grep-$1-$2-clients.log"
echo "Done"

#oarsh $JOBTRACKER "$SCRIPTS_HOME/source env.sh; $HADOOP_HOME/bin/stop-mapred.sh"

#tar cjf $HADOOP_HOME/results/write-$1-run-$i-`date +%y-%m-%d-%H-%M`.tar.bz2 $HADOOP_HOME/logs $HADOOP_HOME/benchmark/apps/grep_logs

