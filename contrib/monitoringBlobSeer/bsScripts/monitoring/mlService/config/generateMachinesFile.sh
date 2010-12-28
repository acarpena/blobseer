#!/bin/bash
# This script generates the list of machines that can be used for the MonaLisa nodes
# Each line will consist of a hostname or an IP

########################################

#find the machines reserved for deployment
oargridstat -l $ML_JOB_ID | sort | uniq | grep "grid5000" > $CONF_DIR/temp.txt

# MonALISA deployment
LINES=`cat $CONF_DIR/temp.txt | wc -l`
sh $ML_CONFIG/MLimageDeploy.sh $LINES $CONF_DIR/temp.txt $ENV_FILE
	

taktuk -d -1 -s -l root -f $CONF_DIR/temp.txt broadcast exec [ "test -e $ML_HOME" ]  | grep "status 0" \
	| cut -d '.' -f 1 | sh $UTIL_SCRIPTS/get_full_name.sh > $MLMACHINES_FILE

