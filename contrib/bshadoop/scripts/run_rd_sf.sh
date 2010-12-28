#!/bin/bash

CONF_DIR=$HADOOP_HOME/conf
LOG_SERVER=`cat $CONF_DIR/log_server.txt`

myri=-myri0
NODE=`hostname -f | cut -d '.' -f 1`
INDEX=`grep -x -n $NODE$myri $TEST_SLAVES | cut -d ':' -f 1 `

$HADOOP_HOME/bin/hadoop ReadChunkFromFile $TEST_FILE $CHUNK_SIZE $INDEX $1 $LOG_SERVER $LOG_SRV_PORT 
