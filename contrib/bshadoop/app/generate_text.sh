#!/bin/bash

#arguments: no of clients, no of maps, what to deploy: hdfs or blobseer

EXAMPLE_JAR=$HADOOP_HOME/build/hadoop-$HADOOP_VERSION-examples.jar
CLIENTS=$1
UNCOMPRESSED_DATA_BYTES=$((${CLIENTS} * 67108864))
NUM_MAPS=$2

JOBTRACKER=`cat $HADOOP_HOME/conf/jobtracker.txt`
CLIENT=`cat $HADOOP_HOME/conf/client.txt`
oarsh $JOBTRACKER "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/stop-mapred.sh"
oarsh $JOBTRACKER "killall java"

$SCRIPTS_HOME/$3-deploy.sh

oarsh $JOBTRACKER "source $SCRIPTS_HOME/env.sh; $HADOOP_HOME/bin/start-mapred.sh"
echo "Deployment done"
sleep 10

oarsh $CLIENT "source $SCRIPTS_HOME/env.sh; ${HADOOP_HOME}/bin/hadoop jar \
  ${EXAMPLE_JAR} randomtextwriter \
  -D test.randomtextwrite.total_bytes=${UNCOMPRESSED_DATA_BYTES} \
  -D test.randomtextwrite.bytes_per_map=$((${UNCOMPRESSED_DATA_BYTES} / ${NUM_MAPS})) \
  -D test.randomtextwrite.min_words_key=10 \
  -D test.randomtextwrite.max_words_key=10 \
  -D test.randomtextwrite.min_words_value=100 \
  -D test.randomtextwrite.max_words_value=100 \
  -D mapred.output.compress=false \
  -outFormat org.apache.hadoop.mapred.TextOutputFormat\
  $HFILE &> $HADOOP_HOME/benchmark/apps/random_logs/randomtextwrite-$1-clients-$2-maps.log"

echo "Done"
