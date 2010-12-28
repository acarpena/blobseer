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
nodes=`cat $MLmachinesFILE | sort | uniq | cut -d "." -f 1 `


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

