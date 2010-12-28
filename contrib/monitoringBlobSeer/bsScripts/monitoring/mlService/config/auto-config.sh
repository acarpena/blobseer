#!/bin/bash
$ML_CONFIG/check_env.sh
if [ $? -ne 0 ]
then
    echo "Environment is not configured properly. Check env.sh and source it."
    exit 1
fi
    

# generate a file containing the list of machines to be used as MonALISA sevices
$ML_CONFIG/generateMachinesFile.sh

LINES=`cat $MLMACHINES_FILE | wc -l`
echo "Checking reservation $ML_JOB_ID: $LINES machines are deployed for the MonALISA services."
rm -rf $CONF_DIR/temp.txt

# Other cleanup: remove ~/.ssh/known_hosts on all nodes
taktuk -d -1 -s -l root -f $MLMACHINES_FILE broadcast exec [ "rm -rf $HOME/.ssh/known_hosts" ] > /dev/null
taktuk -d -1 -s -l blobmon -f $MLMACHINES_FILE broadcast exec [ "rm -rf $HOME/.ssh/known_hosts" ] > /dev/null

# Run the iptables script on the nodes
sh $ML_CONFIG/runIptablesRules.sh $MLMACHINES_FILE

echo "The MonALISA nodes are configured. Don't forget to create the ssh tunnels for the MonALISA nodes before deploying MonALISA"
