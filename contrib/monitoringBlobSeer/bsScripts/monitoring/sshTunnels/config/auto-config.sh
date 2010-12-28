#!/bin/bash

# Create the $SSH tunnels for the MonALISA service nodes


#############################
# Create an array of machines (conected to the network in IRISA) from where the $SSH tunnels can be initialized
irisaNodesList=`cat $IRISAmachinesFILE | sort | uniq`

if [ "$irisaNodesList" = "" ] 
then
    irisaNodesList="localhost"
fi

irisaCOUNT=0
for node in $irisaNodesList
do
    irisaNODES[$irisaCOUNT]=$node
    irisaCOUNT=$(($irisaCOUNT + 1))
done



#############################
# Assign MonALISA nodes to machines from where the tunnels can be initiated
nodes=`cat $MLMACHINES_FILE | sort | uniq | cut -d "." -f 1 `


#copy the tunnels script to the accessible IRISA machine
scp $SCP_OPT create_tunnels.sh $USER@$IRISA_ACCESS:~/
#clean the IRISA machines    
CLEAN="rm -f ~/.ssh/known_hosts"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$CLEAN"

echo "Cleaning IRISA nodes..."

for node in $irisaNodesList
do
	CLEAN="'sh ~/delete_tunnels.sh'"
	cleanTunnels="ssh $SSH_OPT $USER@$node $CLEAN"
	ssh $SSH_OPT $USER@$IRISA_ACCESS "$cleanTunnels"	
done
echo "done"



i=0
for node in $nodes
do
    #assign ports for the tunnels for the current MonALISA node
    # extract a number from the host name
    MACHINE_NUMBER=`echo $node |            \
                    sed "s/grid5000//g" |	\
                    sed "s/[^0-9]//g"
                    `		
    if [ ${#MACHINE_NUMBER} = 1 ] ; then
    MACHINE_NUMBER="0$MACHINE_NUMBER"
    fi
    #create the first port number
    FIRST_PORT="1${MACHINE_NUMBER}00"
    
    # restart from the first irisa node if there are more monALISA machines
    if [ $i -eq $irisaCOUNT ]
    then
        i=0
    fi
    
    #execute the tunnels script on the IRISA machine
    echo "Creating tunnels from $USER@${irisaNODES[$i]} to $node... "
	
	CREATE_TUNNELS="'sh ~/create_tunnels.sh  $node $FIRST_PORT'"
	runTunnels="ssh $SSH_OPT $USER@${irisaNODES[$i]} $CREATE_TUNNELS"
    ssh $SSH_OPT $USER@$IRISA_ACCESS $runTunnels  & 
     
    sleep 10        
    #advance to the next IRISA node
    i=$(($i + 1))
done 

sleep 30
echo 'Tunnels created from the IRISA network to all the MonALISA nodes. Now the MonALISA system can be deployed'




