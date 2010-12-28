#!/bin/bash
if [ "$BSMON_SUPPORT_HOME" == "" ] 
then
    echo "\$BSMON_SUPPORT_HOME is undefined. It must hold the path to the BlobSeer monitoring support code."
    exit 1
fi

if [ "$BSMON_SCRIPTS" == "" ] 
then
    echo "\$BSMON_SCRIPTS is undefined. It must point to the BlobSeer monitoring support scripts directory."
    exit 1
fi

if [ "$APMON_SUPPORT_CFG_FILE" == "" ]
then 
    echo "\$APMON_SUPPORT_CFG_FILE is undefined. It must point to the ApMon support for the BlobSeer config file."
    exit 1
fi

if [ "$APMON_CFG_FILE" == "" ]
then 
    echo "\$APMON_CFG_FILE is undefined. It must point to the ApMon configuration file."
    exit 1
fi
