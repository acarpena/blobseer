#!/bin/bash
# This script generates the list of machines that can be used for the BlobSeer nodes
# Each line will consist of a hostname or an IP for a BlobSeer actor, as follows:
# version manager
# provider manaer
# logging server
# $DHT_NO lines for the metadata providers
# the rest (or only $NO_PROVIDERS) will be used as providers
# 

########################################

oargridstat -l $OAR_JOB_ID | sort | uniq | grep grid5000 >$CONF_DIR/temp.txt

# reservation with oarsub
if [ "$OARSUB" = 'true' ]
then
	cat $INIT_NODES_FILE | sort | uniq | grep grid5000 >$CONF_DIR/temp.txt
fi

# check nodes and create configuration file
taktuk -s -c "oarsh $SSH_OPT" -f $CONF_DIR/temp.txt broadcast exec \
    [ "rm -rf /tmp/blobseer; mkdir /tmp/blobseer" ] | \
    grep "status 0" | cut -d '.' -f 1 | sh $UTIL_SCRIPTS/get_full_name.sh > $CONF_DIR/machines.txt

if [ $USE_MYRI = 'true' ]; then
	cat $CONF_DIR/machines.txt | cut -d '.' -f 1 | sh $UTIL_SCRIPTS/add_myri.sh | sh $UTIL_SCRIPTS/get_full_name.sh > $CONF_DIR/machines_tmp.txt
	cat $CONF_DIR/machines_tmp.txt > $CONF_DIR/machines.txt
	rm -rf $CONF_DIR/machines_tmp.txt
fi
