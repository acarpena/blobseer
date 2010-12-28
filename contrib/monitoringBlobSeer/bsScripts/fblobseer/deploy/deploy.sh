#!/bin/bash

# Clean logs & stuff
$CONNECTOR $SSH_OPT $NAMESPACE_MAN_NODE "source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $FBS_SCRIPTS/env.sh; $FBS_DEPLOY/cleanup.sh"

echo "Start the namespace manager..."
# Run the Log server
$CONNECTOR $SSH_OPT $NAMESPACE_MAN_NODE "source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $FBS_SCRIPTS/env.sh; $FBS_DEPLOY/run_and_log.sh $FBS_HOME/namespaceMan/NamespaceManager $BS_CONFIG_FILE"