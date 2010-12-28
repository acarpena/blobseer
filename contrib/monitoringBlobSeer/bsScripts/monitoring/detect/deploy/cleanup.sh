#!/bin/bash

echo "Cleaning the node...."

# stop the detection module

STOP_DETECT=" pkill MaliciousClientsFeedback; killall MaliciousClientsFeedback"
CLEAN="ssh $SSH_OPT $USER@$DETECT_NODE $STOP_DETECT"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"

STOP_DETECT=" kill -9 `ps ax | grep feedback/userAccounting/MaliciousClientsFeedback | awk '{ print $1 }'` "
CLEAN="ssh $SSH_OPT $USER@$DETECT_NODE $STOP_DETECT"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"

STOP_DETECT=" kill -9 `ps ax | grep MaliciousClientsFeedback | cut -d ' ' -f 1` "
CLEAN="ssh $SSH_OPT $USER@$DETECT_NODE $STOP_DETECT"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"

STOP_DETECT=" kill -9 `ps ax | grep MaliciousClientsFeedback | cut -d ' ' -f 2` "
CLEAN="ssh $SSH_OPT $USER@$DETECT_NODE $STOP_DETECT"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"

STOP_DETECT=" ps ax | grep MaliciousClientsFeedback  | cut -d ' ' -f 2 | awk '{com=\"kill -9 \" $0; system(com) }' "
CLEAN="ssh $SSH_OPT $USER@$DETECT_NODE $STOP_DETECT"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"

STOP_DETECT=" ps ax | grep MaliciousClientsFeedback  | cut -d ' ' -f 1 | awk '{com=\"kill -9 \" $0; system(com) }' "
CLEAN="ssh $SSH_OPT $USER@$DETECT_NODE $STOP_DETECT"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"



sleep 5

echo "done"
