#!/bin/sh

#usage: ./MLimageDeploy.sh nodes node_file env_file
if [ $# -ne 3 ] ; then        # if the number of params is not 3
    echo Usage: $0 nodes node_file env_file
    exit 1
fi

#variables
NODES=$1
NODE_FILE=$2
ENV_FILE=$3

#oarsub -I -t deploy -l'nodes=$NODES'
# check if a reservation exists
DEPL_RESERV=`oarstat -u $USER -f | grep "deploy = 'YES'"`
if [ -z "$DEPL_RESERV" ] ; then
    echo 'No reservation found'
    exit 1    
fi


# deploy the image on the nodes
cat $NODE_FILE | uniq | head -n $NODES > temp_node_file
cat temp_node_file
kadeploy3 -a $ENV_FILE -f temp_node_file > log
rm temp_node_file