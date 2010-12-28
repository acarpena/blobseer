#!/bin/bash
# argument: retry no

CONF_DIR=$HADOOP_HOME/conf
LOG_SERVER=`cat $CONF_DIR/log_server.txt`

NODE=`hostname -f | cut -d '.' -f 1`
myri=-myri0
INDEX=`grep -x -n $NODE$myri $TEST_SLAVES | cut -d ':' -f 1 `
#echo "$HADOOP_HOME/bin/hadoop WriteToFile /test$INDEX.dat 30"
$HADOOP_HOME/bin/hadoop WriteToFile /test$INDEX.dat 30 $1 $LOG_SERVER $LOG_SRV_PORT

