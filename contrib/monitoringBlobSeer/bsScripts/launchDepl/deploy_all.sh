#!/bin/bash

GLOBAL=`sh check_global_config.sh`
echo "Setting the global configuration variables: $GLOBAL"
source $GLOBAL
source $LAUNCH_ALL_SCRIPTS/deployEntities.sh


# Deploying  the selected programs

#Deploying BlobSeer
if [ "$DEPLOY_BS" == "true" ] 
then
    echo "Deploying BlobSeer..."
    source $BS_SCRIPTS/env.sh
    sh $BS_DEPLOY/deploy.sh

echo
echo
fi


#Deploying FBlobSeer
if [ "$DEPLOY_FBS" == "true" ] 
then
    echo "Deploying FBlobSeer..."
    source $FBS_SCRIPTS/env.sh
    sh $FBS_DEPLOY/deploy.sh

echo
echo
fi

#Deploying the Logging Server
if [ "$DEPLOY_LOGSERV" == "true" ] 
then
    echo "Deploying the Logging Server..."
    source $LOGSERV_SCRIPTS/env.sh
    sh $LOGSERV_DEPLOY/deploy.sh

echo
echo
fi


#Deploying MonALISA
if [ "$DEPLOY_MONALISA" == "true" ] 
then
    echo "Deploying MonALISA..."
    source $ML_SCRIPTS/env.sh
	sh $ML_DEPLOY/deploy.sh

echo
echo
fi


#Deploying BlobSeer monitoring support
if [ "$DEPLOY_BSMON" == "true" ] 
then
    echo "Deploying the BlobSeer monitoring support..."
	source $ML_SCRIPTS/env.sh
    source $BSMON_SCRIPTS/env.sh
	sh $BSMON_DEPLOY/deploy.sh
echo
echo
fi


#Deploying BlobSeer Malicious clients
if [ "$DEPLOY_BSCLIENTS" == "true" ] 
then
    echo "Deploying the malicious clients..."
    source $BSCLIENTS_SCRIPTS/env.sh
    sh $BSCLIENTS_DEPLOY/deploy.sh

echo
echo
fi


#configuring the Malicious users detection module
if [ "$DEPLOY_DETECT" == "true" ]
then
    echo "Deploying the detection module..."
    source $DETECT_SCRIPTS/env.sh
    sh $DETECT_DEPLOY/deploy.sh
    if [ "$?" == "0" ]
    then
        echo "Succesfully deployed the detection module"
    else
        echo "The deployment ended with errors"
    fi

echo
echo
fi


#Deploying the MonALISA repository
#if [ "$DEPLOY_MLREPOS" == "true" ] 
#then
#    echo "Deploying the MonALISA repository..."
#    source $MLREPOS_SCRIPTS/env.sh
#    sh $MLREPOS_DEPLOY/deploy.sh
#    if [ "$?" == "0" ]
#    then
#        echo Succesfully deployed the MonALISA repository
#    else
#        echo The MonALISA repository deployment ended with errors
#    fi

#fi




