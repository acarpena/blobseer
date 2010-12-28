#!/bin/bash
if [ "$OAR_JOB_ID" == "" ] 
then
    echo "\$JOB_ID is undefined. It must hold the oargridsub reservation number."
    exit 1
fi

if [ "$OAR_JOB_KEY_FILE" == "" ] 
then
    echo "\$OAR_JOB_KEY_FILE is undefined. It must hold the private key generated by oargridsub."
    exit 1
fi

if [ "$HADOOP_HOME" == "" ]
then 
    echo "\$HADOOP_HOME is undefined. It must point to hadoop home dir."
    exit 1
fi

if [ "$SCRIPTS_HOME" == "" ]
then 
    echo "\$SCRIPTS_HOME is undefined. It must point to hadoop home dir."
    exit 1
fi

if [ "$CLIENT_OUTPUT" == "" ]
then
    echo "\$CLIENT_OUTPUT is undefined. It must point to a file name."
    exit 1
fi

if [ "$TEST_FILE" == "" ]
then
    echo "\$TEST_FILE is undefined. It must point to a file name in hdfs/bsfs."
    exit 1
fi

if [ "$CHUNK_SIZE" == "" ]
then
    echo "\$CHUNK_SIZE is undefined."
    exit 1
fi

if [ "$DATA_NODES" == "" ]
then
    echo "\$DATA_NODES is undefined."
    exit 1
fi
