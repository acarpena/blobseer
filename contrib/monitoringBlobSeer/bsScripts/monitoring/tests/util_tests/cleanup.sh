# force stop java on both nodes

#for node in `cat $IRISAmachinesFILE`
#do
#STOP="\"kill -9 `ps ax | grep java | cut -d ' ' -f 1`  \""
#STOP="\"ps ax | grep java | cut -d ' ' -f 1 | awk '{com=\\\"kill -9 \\\" $0; system(com) }'\""


#STOP="\"sh ~/delete_java.sh\""
#CLEAN="ssh $SSH_OPT acarpena@sabre $STOP"
#ssh $SSH_OPT acarpena@transit.irisa.fr "$CLEAN" &


#STOP="\"sh ~/cleanup-db.sh\""
#CLEAN="ssh $SSH_OPT acarpena@sabre $STOP"
#ssh $SSH_OPT acarpena@transit.irisa.fr "$CLEAN" &

STOP="\"sh ~/delete_tunnels.sh\""
CLEAN="ssh $SSH_OPT acarpena@sabre $STOP"
ssh $SSH_OPT acarpena@transit.irisa.fr "$CLEAN" &


STOP="\"sh ~/cleanup_all.sh\""
CLEAN="ssh $SSH_OPT acarpena@sabre $STOP"
ssh $SSH_OPT acarpena@transit.irisa.fr "$CLEAN" &

STOP="\"sh ~/delete_tunnels.sh\""
CLEAN="ssh $SSH_OPT acarpena@sabre $STOP"
ssh $SSH_OPT acarpena@transit.irisa.fr "$CLEAN" &


STOP="\"sh ~/cleanup_all.sh\""
CLEAN="ssh $SSH_OPT acarpena@sabre $STOP"
ssh $SSH_OPT acarpena@transit.irisa.fr "$CLEAN" &

STOP="\"sh ~/delete_tunnels.sh\""
CLEAN="ssh $SSH_OPT acarpena@sabre $STOP"
ssh $SSH_OPT acarpena@transit.irisa.fr "$CLEAN" &


STOP="\"sh ~/cleanup_all.sh\""
CLEAN="ssh $SSH_OPT acarpena@sabre $STOP"
ssh $SSH_OPT acarpena@transit.irisa.fr "$CLEAN" &


#done
