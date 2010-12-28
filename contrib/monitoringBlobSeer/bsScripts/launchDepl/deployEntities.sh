#!/bin/bash

# set this variable to true if you need to deploy Blobseer
export DEPLOY_BS="true"

# set this variable to true if you need to deploy FBlobseer (the Blobseer namespace manager)
export DEPLOY_FBS="true"

# set this variable to true if you need to deploy the Logging Server
export DEPLOY_LOGSERV="true"

# set this variable to true if you need to deploy 
# the Blobseer monitoring support
export DEPLOY_BSMON="true"

# set this variable to true if you need to deploy the
# BlobSeer malicious clients
export DEPLOY_BSCLIENTS="true"



# set this variable to true if you need to deploy the MonALISA repository
export DEPLOY_MLREPOS="false"

# set this variable to true if you need to deploy the MonALISA repository
export DEPLOY_DETECT="false"


# set this variable to true if you need to deploy MonALISA
export DEPLOY_MONALISA="true"


# typically this variable is true if MonALISA has to be deployed
export DEPLOY_SSH_TUNNELS="false"
if [ "$DEPLOY_MONALISA" = "true" ]
then
    export DEPLOY_SSH_TUNNELS="true"
fi
# uncomment the next line if you want to override the default settings for the SSH Tunnels
#export DEPLOY_SSH_TUNNELS="true"


