#!/bin/bash

if [ "$USING_G5K" == "true" ]
then

for site in $CLUSTERS
do
	ssh $USER@$site "rm -rf $G5K_DEPLOY_HOME/conf/*"
	rsync -avz $G5K_DEPLOY_HOME/conf/ $USER@$site:$G5K_DEPLOY_HOME/conf/
	rsync -avz $BLOBSEER_HOME/ $USER@$site:$BLOBSEER_HOME/
done

else
    # copy configuration files to all the nodes involved in the deployment
    HOSTIP=`ifconfig  | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'`
    CMD="$CPCONNECTOR $USER@$HOSTIP:$G5K_DEPLOY_HOME/conf/* $G5K_DEPLOY_HOME/conf"
    
    taktuk -s -c "$CONNECTOR $SSH_OPT "  -f $MACHINES_FILE broadcast exec [ $CMD ] | grep "status 0" | wc -l
    echo " nodes updated"
fi

sleep 10
