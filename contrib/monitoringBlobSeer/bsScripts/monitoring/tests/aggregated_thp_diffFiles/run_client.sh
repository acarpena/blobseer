#!/bin/bash


# command for running the clients:
CMD="source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $BS_SCRIPTS/env.sh; source $BSCLIENTS_SCRIPTS/env.sh; source $TEST_SCRIPTS/$TEST_NAME/env.sh; sh $BSCLIENTS_DEPLOY/run_and_log.sh sh $TEST_SCRIPTS/$TEST_NAME/conf_run_client.sh"

taktuk -s -d -1 -c "$CONNECTOR $SSH_OPT " -f $CLIENTS_FILE broadcast exec [ $CMD ] | \
       grep "status 0" | wc -l ; echo " clients executed"


sleep 20
echo 'Test completed'
