#!/bin/bash
export ML_JOB_ID=21991
export ML_JOB_KEY_FILE=/tmp/oargrid/oargrid_ssh_key_acarpena_21991

export ML_CONFIG=$ML_SCRIPTS/config
export ML_DEPLOY=$ML_SCRIPTS/deploy

export ENV_FILE=$HOME/mlImage/myenv.env

# the path to MonALISA on the deployed image
export ML_HOME=/home/blobmon/MonaLisa
