#!/bin/bash

# Clean logs & stuff
$CONNECTOR $SSH_OPT $LOG_SERVER "source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $LOGSERV_SCRIPTS/env.sh; $LOGSERV_DEPLOY/cleanup.sh"

echo "Start the logging server..."
# Run the Log server
$CONNECTOR $SSH_OPT $LOG_SERVER "source $G5K_DEPLOY_HOME/globalEnv/env.sh; source $LOGSERV_SCRIPTS/env.sh; cd $LOGGING_SERVER/build; $JAVA_HOME/bin/java LogServer $LOG_SRV_PORT &> $LOG_NAME &"
