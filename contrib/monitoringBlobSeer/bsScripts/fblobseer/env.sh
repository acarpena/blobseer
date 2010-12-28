#!/bin/bash
export FBS_CONFIG=$FBS_SCRIPTS/config
export FBS_DEPLOY=$FBS_SCRIPTS/deploy

export FBS_HOME=$HOME/Work/blobseer/contrib/NamespaceManager

# source the BlobSeer environment conf file to get the name 
# of the log server node
source $BS_SCRIPTS/env.sh

export NAMESPACE_MAN_NODE=`cat $UNUSED_NODES | head -n 1`
export NAMESPACE_MAN_PORT=13000

export LOG_NAME="$RESULTS/FBSer-$HOSTNAME.log"
