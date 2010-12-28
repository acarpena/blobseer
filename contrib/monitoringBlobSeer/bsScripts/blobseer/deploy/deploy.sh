#!/bin/bash
$BS_CONFIG/check_env.sh
if [ $? -ne 0 ]
then 
    echo "Environment not configured properly. Check env.sh and source it."
    exit 1
fi

# Entities
VMANAGER=`head -n 1 $MACHINES_FILE `
PMANAGER=`head -n 2 $MACHINES_FILE | tail -n 1`

echo "Cleanup nodes"
# Clean logs & stuff
taktuk -s -c "$CONNECTOR $SSH_OPT "  -f $MACHINES_FILE broadcast exec [ "rm -rf ~/.ssh/known_hosts" ] > /dev/null
				
taktuk -s -c "$CONNECTOR $SSH_OPT "  -f $MACHINES_FILE broadcast exec [ "$BS_DEPLOY/cleanup.sh" ] > /dev/null
rm -rf $RESULTS

# Start BlobSeer
mkdir -p $RESULTS
echo "Starting the version manager $VMANAGER"
$CONNECTOR $SSH_OPT $VMANAGER "source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $BS_SCRIPTS/env.sh; $BS_DEPLOY/run_and_log.sh $BLOBSEER_HOME/vmanager/vmanager $BS_CONFIG_FILE"

echo "Starting the provider manager $PMANAGER"
$CONNECTOR $SSH_OPT $PMANAGER "source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $BS_SCRIPTS/env.sh; $BS_DEPLOY/run_and_log.sh $BLOBSEER_HOME/pmanager/pmanager $BS_CONFIG_FILE"

echo "Starting the providers..."
CMD="source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $BS_SCRIPTS/env.sh; $BS_DEPLOY/run_and_log.sh $BLOBSEER_HOME/provider/provider $BS_CONFIG_FILE"
taktuk -s -c "$CONNECTOR $SSH_OPT "  -f $G5K_DEPLOY_HOME/conf/providers.txt broadcast exec [ $CMD ] | \
			grep "status 0" | wc -l 
echo " deployed providers"

echo "Starting the metadata providers..."
CMD="source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $BS_SCRIPTS/env.sh; $BS_DEPLOY/run_and_log.sh $BLOBSEER_HOME/provider/sdht $BS_CONFIG_FILE"
taktuk -s -c "$CONNECTOR $SSH_OPT "  -f $G5K_DEPLOY_HOME/conf/meta.txt broadcast exec [ $CMD ] | \
            grep "status 0" | wc -l  
echo " deployed metadata providers"

sleep 20
echo 'BlobSeer Deployment complete'
