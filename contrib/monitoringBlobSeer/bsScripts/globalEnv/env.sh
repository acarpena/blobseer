#!/bin/bash

# Deployment on Grid'5000
export USING_G5K="true"


# Scripts home directory
export G5K_DEPLOY_HOME="$HOME/bsScripts"
export UTIL_SCRIPTS="$G5K_DEPLOY_HOME/util"

# Scripts subfolders
export BS_SCRIPTS=$G5K_DEPLOY_HOME/blobseer
export FBS_SCRIPTS=$G5K_DEPLOY_HOME/fblobseer
export LOGSERV_SCRIPTS=$G5K_DEPLOY_HOME/logging_server
export BSMON_SCRIPTS=$G5K_DEPLOY_HOME/monitoring/BSmonSupport
export BSCLIENTS_SCRIPTS=$G5K_DEPLOY_HOME/monitoring/maliciousClients
export ML_SCRIPTS=$G5K_DEPLOY_HOME/monitoring/mlService
export SSH_TUNNELS_SCRIPTS=$G5K_DEPLOY_HOME/monitoring/sshTunnels
export MLREPOS_SCRIPTS=$G5K_DEPLOY_HOME/monitoring/mlRepository
export LAUNCH_ALL_SCRIPTS=$G5K_DEPLOY_HOME/launchDepl
export DETECT_SCRIPTS=$G5K_DEPLOY_HOME/monitoring/detect
export TEST_SCRIPTS=$G5K_DEPLOY_HOME/monitoring/tests


# SSH options
export CONNECTOR="oarsh"
export CPCONNECTOR="oarcp"
export SSH_OPT=" -o stricthostkeychecking=no "
export SCP_OPT=" -o stricthostkeychecking=no "

# JAVA
export JAVA_HOME="$HOME/deploy/jdk"

# BlobSeer home directory
export BLOBSEER_HOME="$HOME/Work/blobseer/tags/release-0.3.1"

export USER=acarpena
export CLUSTERS=""

# Local directory on the nodes
export LOCAL_DIR="/tmp"


export RESULTS="$G5K_DEPLOY_HOME/results/logs"


