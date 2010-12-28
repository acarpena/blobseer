#!/bin/bash
$BS_CONFIG/check_env.sh
if [ $? -ne 0 ]
then
    echo "Environment is not configured properly. Check env.sh and source it."
    exit 1
fi
    

#######################################
# Generate a file $MACHINES_FILE in the configuration folder, with the names of the machines which will serve as blobseer nodes
$BS_CONFIG/generateMachinesFile.sh
    


########################################
# BlobSeer configuration: the first 3 nodes for vmanager, provider manager, and logging server
# the next $DHT_NO nodes for DHT nodes and the others for providers 
# if the PROVIDERS_NO variable is not set.

# Create files with the version manager and the provider maanger
head -n 1 $MACHINES_FILE  > $CONF_DIR/vmanager.txt
head -n 2 $MACHINES_FILE | tail -n 1 > $CONF_DIR/pmanager.txt

NODES=`cat $MACHINES_FILE | wc -l`
head -n `expr $DHT_NO + 3` $MACHINES_FILE | tail -n $DHT_NO > $CONF_DIR/meta.txt

if [ "$PROVIDERS_NO" = "" ]
then 
    cat $MACHINES_FILE | tail -n `expr $NODES - $DHT_NO - 3` > $CONF_DIR/providers.txt
    echo > $CONF_DIR/unused_nodes.txt
else
# If the PROVIDERS_NO varible is set, the remaining nodes will be put in a file containing unused nodes.
    cat $MACHINES_FILE | tail -n `expr $NODES - $DHT_NO - 3`|  \
                head -n $PROVIDERS_NO > $CONF_DIR/providers.txt
    cat $MACHINES_FILE | tail -n `expr $NODES - $DHT_NO - 3 - $PROVIDERS_NO` > $CONF_DIR/unused_nodes.txt
fi

# Create a file with the Logging server
# (the third node)
cat $MACHINES_FILE | head -n 3 | tail -n 1 > $LOG_SERVER_FILE

echo Checking reservation $OAR_JOB_ID: $NODES machines out of `cat $MACHINES_FILE | grep grid5000 | wc -l` are configured properly and usable
rm -rf $CONF_DIR/temp.txt 

if [ "$NODES" == "0" ]
then
    exit 1
fi


########################################
# Create a temp file with the version manager, provider manager and DHT nodes to generate the configuration file
head -n 2 $MACHINES_FILE >$CONF_DIR/temp.txt
cat $CONF_DIR/meta.txt >> $CONF_DIR/temp.txt

# remove the old configuration file
rm -f $BS_CONFIG_FILE

$BS_CONFIG/gen_cfgfile.pl $CONF_DIR/temp.txt $DHT_NO $BS_CONFIG_FILE
rm -rf $CONF_DIR/temp.txt


# Other cleanup: remove ~/.ssh/known_hosts on all nodes
taktuk -s -c "$CONNECTOR SSH_OPT" -f  $MACHINES_FILE broadcast exec [ "rm -rf $HOME/.ssh/known_hosts" ] > /dev/null



