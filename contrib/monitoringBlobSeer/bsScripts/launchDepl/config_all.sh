#!/bin/bash

#rm -rf $HOME/.ssh/known_hosts

GLOBAL=`sh check_global_config.sh`
echo "Setting the global configuration variables: $GLOBAL"
source $GLOBAL

source $LAUNCH_ALL_SCRIPTS/deployEntities.sh


# configuring  the selected programs

#configuring BlobSeer
if [ "$DEPLOY_BS" == "true" ] 
then
    echo "Configuring BlobSeer..."
    source $BS_SCRIPTS/env.sh
    sh $BS_CONFIG/auto-config.sh
    if [ "$?" == "0" ]
    then
	echo Succesfully configured Blobseer
    else
	echo BlobSeer configuration ended with errors
    fi 

echo
echo
fi

#configuring BlobSeer namespace manager
if [ "$DEPLOY_FBS" == "true" ] 
then
    echo "Configuring FBlobSeer..."
    source $FBS_SCRIPTS/env.sh
    sh $FBS_CONFIG/auto-config.sh
    if [ "$?" == "0" ]
    then
	echo Succesfully configured FBlobseer
    else
	echo FBlobSeer configuration ended with errors
    fi 

echo
echo
fi


#configuring the Logging Server
if [ "$DEPLOY_LOGSERV" == "true" ] 
then
    echo "Configuring the Logging Server..."
    source $LOGSERV_SCRIPTS/env.sh
    sh $LOGSERV_CONFIG/auto-config.sh
    if [ "$?" == "0" ]
    then
        echo Succesfully configured the Logging Server
    else
        echo The Logging Server configuration ended with errors
    fi

echo
echo
fi


#configuring MonALISA
if [ "$DEPLOY_MONALISA" == "true" ]
then
    echo "Configuring MonALISA..."
    source $ML_SCRIPTS/env.sh
    sh $ML_CONFIG/auto-config.sh
    if [ "$?" == "0" ]
    then
        echo Succesfully configured MonALISA
    else
        echo MonALISA configuration ended with errors
    fi

echo
echo
fi


#configuring the SSH Tunnels
if [ "$DEPLOY_SSH_TUNNELS" == "true" ] 
then
    echo "Configuring the SSH tunnels..."
    source $SSH_TUNNELS_SCRIPTS/env.sh
    sh $SSH_TUNNELS_CONFIG/auto-config.sh
    if [ "$?" == "0" ]
    then
        echo Succesfully configured the SSH tunnels
    else
        echo The SSH Tunnels configuration ended with errors
    fi

echo
echo
fi


#configuring BlobSeer monitoring support
if [ "$DEPLOY_BSMON" == "true" ] 
then
    echo "Configuring the BlobSeer monitoring support..."
    source $BSMON_SCRIPTS/env.sh
    sh $BSMON_CONFIG/auto-config.sh
    if [ "$?" == "0" ]
    then
        echo Succesfully configured the Blobseer monitoring support
    else
        echo BlobSeer monitoring support configuration ended with errors
    fi

echo
echo
fi


#configuring BlobSeer Malicious clients
if [ "$DEPLOY_BSCLIENTS" == "true" ] 
then
    echo "Configuring the BlobSeer malicious clients..."
    source $BSCLIENTS_SCRIPTS/env.sh
    sh $BSCLIENTS_CONFIG/auto-config.sh
    if [ "$?" == "0" ]
    then
        echo Succesfully configured the Blobseer Malicious clients
    else
        echo BlobSeer Malicious clients configuration ended with errors
    fi

echo
echo
fi


#configuring the MonALISA repository
if [ "$DEPLOY_MLREPOS" == "true" ] 
then
    echo "Configuring the MonALISA repository..."
    source $MLREPOS_SCRIPTS/env.sh
    sh $MLREPOS_CONFIG/auto-config.sh
    if [ "$?" == "0" ]
    then
        echo Succesfully configured the MonALISA repository
    else
        echo The MonALISA repository configuration ended with errors
    fi

echo
echo
fi


#configuring the Malicious users detection module
if [ "$DEPLOY_DETECT" == "true" ]
then
    echo "Configuring the detection module..."
    source $DETECT_SCRIPTS/env.sh
    sh $DETECT_CONFIG/auto-config.sh
    if [ "$?" == "0" ]
    then
        echo Succesfully configured the detection module
    else
        echo The configuration ended with errors
    fi

echo
echo
fi


# rsync with other sites

# rsync with other sites
echo "Copying configuration files to all the sites..."
$UTIL_SCRIPTS/sendConfigurationFile.sh



