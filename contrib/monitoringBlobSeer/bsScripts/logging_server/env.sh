#!/bin/bash
export LOGSERV_CONFIG=$LOGSERV_SCRIPTS/config
export LOGSERV_DEPLOY=$LOGSERV_SCRIPTS/deploy

export LOGGING_SERVER=$HOME/Work/blobseer/contrib/logging_service/logging_server_java

# source the BlobSeer environment conf file to get the name 
# of the log server node
source $BS_SCRIPTS/env.sh

export LOG_SERVER=`cat $LOG_SERVER_FILE`
export LOG_SRV_PORT=9000

export LOG_NAME="$RESULTS/logServer-$HOSTNAME.log"
