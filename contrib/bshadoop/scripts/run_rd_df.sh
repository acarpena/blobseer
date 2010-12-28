#!/bin/bash
#arguments: no of retries

CONF_DIR=$HADOOP_HOME/conf
LOG_SERVER=`cat $CONF_DIR/log_server.txt`

myri=-myri0
NODE=`hostname -f | cut -d '.' -f 1`
INDEX=`grep -x -n $NODE$myri $TEST_SLAVES | cut -d ':' -f 1 `
NODES=`cat $TEST_SLAVES | wc -l`
$HADOOP_HOME/bin/hadoop ReadFromFile /test`expr $INDEX % $NODES + 1`.dat 30 $1 $LOG_SERVER $LOG_SRV_PORT

#$HADOOP_HOME/bin/hadoop ReadFromFile /test$INDEX.dat 30 $LOG_SERVER $LOG_SRV_PORT

