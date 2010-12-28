 #!/bin/sh
# written by David Margery for the 2006 Grid'5000 spring school
# feel free to copy and adapt
# updated by Cyril Constantin
# update by David Margery, 2008, to accept parameters

# Parameters

# set your public key here :
KEY_FILE=~/.ssh/id_rsa.pub

####


echo -n "Script running on:" 
hostname

#Get the eventual parameters
if [ $# -gt 0 ] ; then
  ENVIRONMENT="$1"	
else
  ENVIRONMENT='Bs_fuse_image'
fi
echo "attempt to deploy environment $ENVIRONMENT"

#$OAR_FILE_NODES file may contain some nodes more than once
# We will create a file where each node appears one time 

sort -u $OAR_FILE_NODES > ~/OAR.$OAR_JOB_ID.nodes

#deploy or test environnement (provided all the nodes 
# are from the same cluster)
kadeploy -e $ENVIRONMENT -f ~/OAR.$OAR_JOB_ID.nodes

#add your public ssh key to be able to connect to that environment
if [ -f "$KEY_FILE" ] ; then
  kaaddkeys -k $KEY_FILE -f ~/OAR.$OAR_JOB_ID.nodes
else
  echo "Could not add $KEY_FILE to the environment : no such file"
fi

#set common ssh and scp options
#   prevent script waiting for input with BatchMode=yes
SSH_OPTS=' -o StrictHostKeyChecking=no -o BatchMode=yes '


#do stuff with this deployed environment
for node in `cat ~/OAR.$OAR_JOB_ID.nodes`
do 
 echo attempt to get information from $i
 ssh root@$node $SSH_OPTS cat /etc/hostname
 ssh root@$node $SSH_OPTS uname -a
done

if [ $# -gt 1 ] ; then
 if [ -x "$2" ] ; then
     #we elect a head node to run the script
     HEADNODE=`head -1 $OAR_FILE_NODES`
     #we copy the script to the headnode
     scp  $SSH_OPTS $2 root@$HEADNODE:
     exec ssh $SSH_OPT root@$HEADNODE ./`basename $2`
 else
     echo Could not find $2 to run on $HEADNODE
 fi
fi
