#!/bin/bash

source $BS_SCRIPTS/env.sh
source $LOGSERV_SCRIPTS/env.sh


# Add the logging server host and port in the BlobSeer configuration file
echo >> $BS_CONFIG_FILE
echo "logging_client: {" >> $BS_CONFIG_FILE
echo "    host = \"$LOG_SERVER\";">> $BS_CONFIG_FILE
echo "    port = \"$LOG_SRV_PORT\";">> $BS_CONFIG_FILE
echo "};">> $BS_CONFIG_FILE
echo >> $BS_CONFIG_FILE



