#!/bin/bash

export CONF_DIR=$G5K_DEPLOY_HOME/conf

# source the Repository configuration file
source $MLREPOS_SCRIPTS/env.sh

# scripts directories
export DETECT_CONFIG=$DETECT_SCRIPTS/config
export DETECT_DEPLOY=$DETECT_SCRIPTS/deploy

# the node where the malicious users detection module runs on is
# the last node oin the IRISA machines list
export DETECT_NODE="sabre"
#export DETECT_NODE="paralapeche.irisa.fr"
	
# path to the detection module's code
export DETECT_HOME="/udd/acarpena/Work/blobseer/contrib/monitoringBlobSeer/UserAccounting/MaliciousClients"

export CLASSPATH="$DETECT_HOME/lib/log4j-1.2.15.jar:$REPOS_HOME/tomcat/common/lib/BSMonitoring.jar:$REPOS_HOME/lib/JStoreClient.jar:$REPOS_HOME/lib/backport-util-concurrent.jar:$DETECT_HOME/lib/postgresql-8.4-701.jdbc4.jar:."
export JAVA_HOME="/udd/acarpena/utils/jdk1.6.0_17"


# update delay (msec)
export UPDATE_DELAY="400"
 
# port
export DETECT_PORT="15500"

# log
export DETECT_LOCAL="/local"
export DETECT_LOG_NAME=$DETECT_LOCAL/detect-$HOSTNAME-`date +%y-%m-%d-%H-%M`.log

# App.properties path
export APP_PROPS=$REPOS_HOME/JStoreClient/conf/App.properties
