#!/bin/bash
export OAR_JOB_ID=23488
export OAR_JOB_KEY_FILE=$HOME/keys/oargrid_ssh_key_dmoise_23488
export HADOOP_HOME=$HOME/hadoop-yahoo
export SCRIPTS_HOME=$HADOOP_HOME/scripts/app
export CLIENT_OUTPUT=/tmp/rez-$OAR_JOB_ID
export TEST_FILE=/test.file-$OAR_JOB_ID
export CHUNK_SIZE=26
export DATA_NODES=$HADOOP_HOME/conf/providers.txt
export TEST_SLAVES=$SCRIPTS_HOME/slaves-$OAR_JOB_ID
export BLOBSEER_HOME=$HOME/blobseer/tags/release-0.3.1
export DHT_NO=10
export HADOOP_VERSION=0.20.0-2787265
export USER=dmoise
export CLUSTER=orsay
export LD_LIBRARY_PATH=$HADOOP_HOME/blobseer-fsmeta/lib/:$LD_LIBRARY_PATH
export LOGGING_SERVER=$HOME/logging_server
export LOG_SRV_PORT=9001
export FILE=$HADOOP_HOME/inputs/fis
export HFILE=/fis

