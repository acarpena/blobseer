#!/bin/bash

echo "Starting the Malicious Client Detection module....$APP_PROPS"

#start the detection module
START_DETECT=" 'setenv PATH $JAVA_HOME/bin:$PATH; cd $DETECT_HOME/build; java -cp $CLASSPATH feedback/userAccounting/MaliciousClientsFeedback $DETECT_PORT $APP_PROPS $UPDATE_DELAY > $DETECT_LOG_NAME & ' "
CONNECT="ssh $SSH_OPT $USER@$DETECT_NODE $START_DETECT "
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CONNECT" &

sleep 5
echo "done"
