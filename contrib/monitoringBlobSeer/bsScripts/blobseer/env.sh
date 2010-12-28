#!/bin/bash

# Job ID
export OAR_JOB_ID=23480
export OAR_JOB_KEY_FILE=$G5K_DEPLOY_HOME/keys/oargrid_ssh_key_acarpena_23480

# Scripts paths
export BS_CONFIG=$BS_SCRIPTS/config
export BS_DEPLOY=$BS_SCRIPTS/deploy
export CONF_DIR=$G5K_DEPLOY_HOME/conf

# oarsub reservation - use a node file
export OARSUB="false"
#export INIT_NODES_FILE=$G5K_DEPLOY_HOME/nodes_file


# Configuration Files
export BS_TEMPLATE_FILE=$G5K_DEPLOY_HOME/conf/bs_template.cfg
export BS_CONFIG_FILE=$G5K_DEPLOY_HOME/conf/running.cfg

# BlobSeer nodes
export DHT_NO=1
export PROVIDERS_NO=3

# Use Myrinet
export USE_MYRI="false"

# The Machines File
export MACHINES_FILE=$CONF_DIR/machines.txt

# Node Files paths
export LOG_SERVER_FILE=$CONF_DIR/log_server.txt
export MACHINES=$CONF_DIR/machines.txt
export DHTS=$CONF_DIR/meta.txt
export PROVIDERS=$CONF_DIR/providers.txt
export UNUSED_NODES=$CONF_DIR/unused_nodes.txt

# Logs
host=`echo $HOSTNAME | sh $UTIL_SCRIPTS/get_full_name.sh`
dt=`date +%H%M%S%N`
export LOG_NAME="$RESULTS/blobseer-$host-$dt.log"

