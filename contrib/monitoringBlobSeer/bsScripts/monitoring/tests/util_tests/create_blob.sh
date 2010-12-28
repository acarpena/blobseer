#!/bin/bash

CREATE_NODE=`head -n 1 $G5K_DEPLOY_HOME/conf/unused_nodes.txt`

for blobs in `seq $BLOBS`
do
    echo "Creating blob $blobs on $CREATE_NODE"
	CMD="source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $BS_SCRIPTS/env.sh; source $BSCLIENTS_SCRIPTS/env.sh; $BSCLIENTS_DEPLOY/run_and_log.sh $CREATE_BLOBS_PROGRAM $CREATE_BLOBS_PARAMS"
    $CONNECTOR $SSH_OPT  $CREATE_NODE $CMD > /dev/null
	sleep 5
done
echo 'Blobs creation completed'


