#!/bin/bash

export SSH_TUNNELS_CONFIG=$SSH_TUNNELS_SCRIPTS/config
export SSH_TUNNELS_DEPLOY=$SSH_TUNNELS_SCRIPTS/deploy

export CONF_DIR=$G5K_DEPLOY_HOME/conf
export IRISAmachinesFILE=$CONF_DIR/IRISAmachines.txt

# the ssh command to connect to the IRISA machines
export IRISA_ACCESS='transit.irisa.fr'


# get the variables from the MonALISA deployment component
source $ML_SCRIPTS/env.sh