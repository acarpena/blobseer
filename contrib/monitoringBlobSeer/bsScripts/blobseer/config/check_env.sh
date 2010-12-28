#!/bin/bash
if [ "$OAR_JOB_ID" == "" ] 
then
    echo "\$JOB_ID is undefined. It must hold the oargridsub reservation number."
    exit 1
fi

#if [ "$OAR_JOB_KEY_FILE" == "" ] 
#then
#    echo "\$OAR_JOB_KEY_FILE is undefined. It must hold the private key generated by oargridsub."
#    exit 1
#fi

if [ "$G5K_DEPLOY_HOME" == "" ]
then 
    echo "\$G5K_DEPLOY_HOME is undefined. It must point to the scripts home directory."
    exit 1
fi

if [ "$BS_SCRIPTS" == "" ]
then 
    echo "\$BS_SCRIPTS is undefined. It must point to the blobseer scripts directory."
    exit 1
fi

if [ "$BS_CONFIG" == "" ]
then
    echo "\$BS_CONFIG is undefined. It must point to the directory where the configuration files for blobseer are stored."
    exit 1
fi

if [ "$BS_DEPLOY" == "" ]
then
    echo "\$BS_DEPLOY is undefined. It must point to the directory where the deployment files for blobseer are stored."
    exit 1
fi

if [ "$BS_TEMPLATE_FILE" == "" ]
then
    echo "\$BS_TEMPLATE_FILE is undefined. It must point to the template file for BlobSeer."
    exit 1
fi


if [ "$LOG_NAME" == "" ]
then
    echo "\$LOG_NAME is undefined. It must contain the path and name template for the BlobSeer logs."
    exit 1
fi
