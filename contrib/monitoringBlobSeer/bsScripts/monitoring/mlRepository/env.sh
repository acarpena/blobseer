#!/bin/bash

source $BS_SCRIPTS/env.sh

export MLREPOS_CONFIG=$MLREPOS_SCRIPTS/config
export MLREPOS_DEPLOY=$MLREPOS_SCRIPTS/deploy

export CONF_DIR=$G5K_DEPLOY_HOME/conf


# the access machine to the IRISA network
export IRISA_ACCESS='transit.irisa.fr'

# the repository will run on the first node from the list of IRISA machines
export IRISAmachinesFILE=$CONF_DIR/IRISAmachines.txt
#export REPOS_NODE=`head -n 2 $IRISAmachinesFILE | tail -n 1`
export REPOS_NODE="sabre"
export DB_NODE=`head -n 1 $IRISAmachinesFILE`

# the path to the repository
export REPOS_HOME="/udd/acarpena/MLrepository"

# the path to the Postgres server
export DB_HOME="/udd/acarpena/postgres"

# the path to the directory where the data are stored
export DATA_DIR="/local/pgsql/data"
export DEFAULT_DATA_PATH="/udd/acarpena/bsScripts/postgres_defaults/pgsql"

# the name of the database
export DB_NAME=bsmon$OAR_JOB_ID_`date +%y_%m_%d_%H_%M`
export DB_SERVER="$DB_NODE"
export DB_PORT="5432"
export DB_PASS=""

# default configuration file
export REPOS_DEFAULT_CONFIGS="$MLREPOS_CONFIG/reposPropDefault"
# the configuration file which will be copied on the repository
export APP_PROPS_FILE="$CONF_DIR/App.properties"
