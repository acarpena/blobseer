#!/bin/bash

# Configure the MonALISA repository

echo "Cleaning the node...."


#stop the database server 

#STOP_DB=" '$DB_HOME/bin/pg_ctl stop -D $DATA_DIR' "
#CLEAN="ssh $SSH_OPT $USER@$DB_NODE $STOP_DB"
#ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"

# force stop database server on the repository node and db node 
STOP_DB="' sh ~/cleanup-db.sh '"
CLEAN="ssh $SSH_OPT $USER@$DB_NODE $STOP_DB"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"

#CLEAN="ssh $SSH_OPT $USER@$REPOS_NODE $STOP_DB"
#ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"

# move the data folder
MOVE_DATA="'cp -rf $DEFAULT_DATA_PATH /local/ '"
MOVE="ssh $SSH_OPT $USER@$DB_NODE $MOVE_DATA"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$MOVE"



sleep 10
echo "done"


echo "Starting the database Server...."

#start the data base server
START_DB=" '/udd/acarpena/postgres/bin/pg_ctl start -D $DATA_DIR > /dev/null' "
CONNECT="ssh $SSH_OPT $USER@$DB_NODE $START_DB "
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CONNECT" 

sleep 20

#create new data base
CREATE_DB=" '$DB_HOME/bin/psql -d template1 -c\"CREATE DATABASE $DB_NAME;\"' "
CONNECT="ssh $SSH_OPT $USER@$DB_NODE $CREATE_DB"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CONNECT"
sleep 5

echo "done"



