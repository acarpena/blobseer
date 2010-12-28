#!/bin/bash

#$HADOOP_HOME/bin/hadoop AppendToFile $TEST_FILE $CHUNK_SIZE >> $CLIENT_OUTPUT


CONF_DIR=$HADOOP_HOME/conf
LOG_SERVER=`cat $CONF_DIR/log_server.txt`

$HADOOP_HOME/bin/hadoop AppendToFile $TEST_FILE $CHUNK_SIZE $1 $LOG_SERVER $LOG_SRV_PORT


