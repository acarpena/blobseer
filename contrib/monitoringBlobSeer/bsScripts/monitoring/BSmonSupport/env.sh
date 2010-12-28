#!/bin/bash

#BlobSeer monitoring support directories
export BSMON_SUPPORT_HOME=$HOME/Work/blobseer/contrib/monitoringBlobSeer/BSmonSupport/BSmonSupport-0.3.4
export BSMON_VMAN_PARSER=$BSMON_SUPPORT_HOME/vmanager

#BlobSeer monitoring support scripts
export BSMON_CONFIG=$BSMON_SCRIPTS/config
export BSMON_DEPLOY=$BSMON_SCRIPTS/deploy

# ApMon support for the BlobSeer config file
export APMON_SUPPORT_CFG_FILE=$BSMON_SUPPORT_HOME/conf/apMonSupport.cfg

# ApMon configuration file
export APMON_CFG_FILE=$BSMON_SUPPORT_HOME/conf/monitor.conf

# Choose the BlobSeer components for which the monitoring support is enabled
export MONITOR_PROVIDERS="true"
export MONITOR_METADTA_PROVIDERS="true"
export MONITOR_PROVIDER_MANAGER="true"
export MONITOR_VERSION_MANAGER="true"

# How many duplicate monitoring parameters to send
export DUPLICATES=1

# Wait time for the vmanager parser
export SLEEP_TIME=5000

# The log for the vamanager parser
export VMANPARSER_LOG_NAME=$RESULTS/vman-parser-$HOSTNAME.log

# The blobseer settings are needed
source $BS_SCRIPTS/env.sh

# The MonALISA settings are needed
source $ML_SCRIPTS/env.sh