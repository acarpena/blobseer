#!/bin/bash

#############################
# Add the namespace manager data to the configuration file

echo "### Namespace manager configuration ####" >> $BS_CONFIG_FILE
echo "NamespaceServer: {" >> $BS_CONFIG_FILE
echo "    host = \"$NAMESPACE_MAN_NODE\";" >> $BS_CONFIG_FILE
echo "    port = \"$NAMESPACE_MAN_PORT\";" >> $BS_CONFIG_FILE
echo "};" >> $BS_CONFIG_FILE

###############################
