#!/bin/bash

#Malicious clients directories
export BSCLIENTS_HOME=$HOME/Work/blobseer/contrib/monitoringBlobSeer/UserAccounting/testClients
export BSCLIENTS_TESTCLIENT=$BSCLIENTS_HOME/client_tests

#Malicious clients scripts
export BSCLIENTS_CONFIG=$BSCLIENTS_SCRIPTS/config
export BSCLIENTS_DEPLOY=$BSCLIENTS_SCRIPTS/deploy

export CLIENT_LOG_NAME=$RESULTS/client-$HOSTNAME.log

# blobseer settings
source $BS_SCRIPTS/env.sh

# logging server settings
source $LOGSERV_SCRIPTS/env.sh


