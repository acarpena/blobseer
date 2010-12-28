#!/bin/bash
$BSMON_CONFIG/check_env.sh
if [ $? -ne 0 ]
then 
    echo "Environment not configured properly. Check env.sh and source it."
    exit 1
fi


# Entities
VMANAGER_NODE=`head -n 1 $MLMACHINES_FILE`


# cleanup
#$CONNECTOR $SSH_OPT  $VMANAGER_NODE "rm -rf ~/.ssh/known_hosts"  > /dev/null


# Start the Version MAnager parser
$CONNECTOR $SSH_OPT $VMANAGER_NODE "source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $BSMON_SCRIPTS/env.sh; cd $BSMON_VMAN_PARSER; $BSMON_DEPLOY/run_and_log.sh $JAVA_HOME/bin/java -cp $BSMON_VMAN_PARSER/lib/apmon.jar:. vmonitor/VManagerMonitor $RESULTS/blobseer-$VMANAGER_NODE.log $G5K_DEPLOY_HOME/conf/monitor-vman.conf $SLEEP_TIME" &

echo 'BlobSeer Monitoring Support Deployment complete'
