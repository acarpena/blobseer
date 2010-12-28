#!/bin/bash

# usage: sh archive_logs.sh test_name

source $MLREPOS_SCRIPTS/env.sh


# get the logs from other sites
echo "Gathering the logs ..."
for site in $CLUSTERS
do
	scp $SCP_OPT -r $USER@$site:$RESULTS/*.log $RESULTS/
done


COMM="\"cp  /local/detect* $RESULTS/\""
NC="ssh $SSH_OPT $USER@$REPOS_NODE $COMM"
ssh $SSH_OPT $USER@$IRISA_ACCESS "$NC"
scp $SCP_OPT -r $USER@$IRISA_ACCESS:$RESULTS/ $RESULTS/

echo "Waiting ..." 
sleep 20

# Archive the logs
echo "Archiving ..."
tar cjf $ARCHIVES/$1-logs-`date +%y-%m-%d-%H-%M`.tar.bz2 $RESULTS


echo "Deleting logs ..."
rm -f $RESULTS/*.log

# delete the logs from other sites
for site in $CLUSTERS
do
    ssh $SSH_OPT $USER@$site "rm -rf $RESULTS/*.log"
done

#COMM="ssh $SSH_OPT $USER@$REPOS_NODE 'rm -rf /local/detect*.log'"
#ssh $SSH_OPT $USER@$IRISA_ACCESS  "$COMM"
