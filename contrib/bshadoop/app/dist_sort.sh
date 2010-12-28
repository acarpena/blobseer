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

mkdir -p $HADOOP_HOME/benchmark/apps/sort_logs

oarsh $CLIENT "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/hadoop fs -put $FILE $HFILE"

oarsh $CLIENT "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/hadoop fs -rmr $HFILE/_logs; $HADOOP_HOME/bin/hadoop fs -ls $HFILE"

#oarsh $CLIENT "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/hadoop fs -touchz $TEST_FILE > /dev/null"

sleep 10

echo "Starting sort..."
oarsh $CLIENT "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/hadoop jar $EXAMPLE_JAR sort -inFormat org.apache.hadoop.mapred.KeyValueTextInputFormat -outFormat org.apache.hadoop.mapred.TextOutputFormat -outKey org.apache.hadoop.io.Text -outValue org.apache.hadoop.io.Text $HFILE output &> $HADOOP_HOME/benchmark/apps/sort_logs/sort-$1-$2-clients.log"
echo "Done"

