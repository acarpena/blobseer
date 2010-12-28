#!/bin/bash

# Configure the MonALISA repository

echo "Cleaning the node...."

# stop the repository
STOP_REPOS=" 'sh $REPOS_HOME/stop.sh' "
CLEAN="ssh $SSH_OPT $USER@$REPOS_NODE $STOP_REPOS"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"



echo "Starting the database server..."
sh $MLREPOS_CONFIG/db-config.sh

#############################
# Create the configuration file for the repository

cat $REPOS_DEFAULT_CONFIGS > $APP_PROPS_FILE

echo "################# Database configuration ################################" >> $APP_PROPS_FILE
echo "lia.Monitor.jdbcDriverString = org.postgresql.Driver" >> $APP_PROPS_FILE
echo "lia.Monitor.ServerName = $DB_SERVER" >> $APP_PROPS_FILE
echo "lia.Monitor.DatabaseName = $DB_NAME" >> $APP_PROPS_FILE
echo "lia.Monitor.DatabasePort = $DB_PORT" >> $APP_PROPS_FILE
echo "lia.Monitor.UserName = $USER" >> $APP_PROPS_FILE
echo "lia.Monitor.Pass = $DB_PASS" >> $APP_PROPS_FILE

# copy the configuration file in the repository
scp $SCP_OPT $APP_PROPS_FILE $USER@$IRISA_ACCESS:$REPOS_HOME/JStoreClient/conf/App.properties


###############################
# Start the MonALISA repository
echo "Starting the MonALISA repository...."

START_REPOS=" 'sh $REPOS_HOME/start.sh' "
COMM="ssh $SSH_OPT $USER@$REPOS_NODE $START_REPOS &"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$COMM"

sleep 5
echo "done"


