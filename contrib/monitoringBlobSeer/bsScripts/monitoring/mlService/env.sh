#!/bin/bash


# Settings for a Grid5000 deployment 
export ML_JOB_ID=23481
export ML_JOB_KEY_FILE=/tmp/oargrid/oargrid_ssh_key_acarpena_23481

# path to the MonaLisa image environment
export ENV_FILE=$HOME/mlImage/myenv.env

##################################################

export ML_CONFIG=$ML_SCRIPTS/config
export ML_DEPLOY=$ML_SCRIPTS/deploy
export CONF_DIR=$G5K_DEPLOY_HOME/conf

# The MonaLisa Machines File
export MLMACHINES_FILE=$CONF_DIR/MLmachines.txt


# the path to MonALISA
export ML_HOME=/home/blobmon/MonaLisa



