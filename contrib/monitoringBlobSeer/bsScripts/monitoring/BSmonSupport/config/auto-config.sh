#!/bin/bash
$BSMON_CONFIG/check_env.sh
if [ $? -ne 0 ]
then
    echo "Environment is not configured properly. Check env.sh and source it."
 #   exit 1
fi
    
CONF_DIR=$G5K_DEPLOY_HOME/conf


#add the ApMon configuration to the template configuration file
TEMPLATE_FILE=$BS_CONFIG_FILE

cat $APMON_SUPPORT_CFG_FILE >> $TEMPLATE_FILE



#add the running MonALISA services to ApMon monitoring configuration file

#remove old config files
rm -rf $CONF_DIR/monitor-*
rm -rf $CONF_DIR/providers-*
rm -rf $CONF_DIR/metaproviders-*
rm -rf $CONF_DIR/pmanager-*

cd $CONF_DIR

# create the needed number of configuration files for ApMon (for providers)
cat $MLMACHINES_FILE | sort | uniq | awk -v mlmachines="$MLMACHINES_FILE"   \
                                         -v duplicates="$DUPLICATES"        \
                                         -f $BSMON_CONFIG/createConfigFiles.awk


if [ "$MONITOR_PROVIDERS" == "true" ] 
then
	#create files with the list of nodes corresponding to each configuration file
    cat $CONF_DIR/providers.txt | sort | uniq |                                     \
        awk -f $BSMON_CONFIG/splitNodes.awk                                     \
        -v lines="`cat $G5K_DEPLOY_HOME/conf/MLmachines.txt | sort | uniq | wc -l`" \
        -v output="providers"
        
        
    # copy the configuration file belonging to each node on its local disk
    cat $MLMACHINES_FILE | sort | uniq | sh $UTIL_SCRIPTS/get_full_name.sh |    \
                    awk -v conf=$CONF_DIR                                       \
                        -v output="providers"                                   \
                        -f $BSMON_CONFIG/sendConfigFile.awk

    sleep 3
    
    # copy the monitoring provider on top of the blobseer provider
    mv $BLOBSEER_HOME/provider/provider $BLOBSEER_HOME/provider/provider.init
    cp $BSMON_SUPPORT_HOME/provider/provider $BLOBSEER_HOME/provider/provider

fi


if [ "$MONITOR_METADATA_PROVIDERS" == "true" ] 
then
	#create files with the list of nodes corresponding to each configuration file
    cat $CONF_DIR/meta.txt | sort | uniq |                                     \
        awk -f $BSMON_CONFIG/splitNodes.awk                                     \
        -v lines="`cat $G5K_DEPLOY_HOME/conf/MLmachines.txt | sort | uniq | wc -l`" \
        -v output="metaproviders"
        
        
    # copy the configuration file belonging to each node on its local disk
    cat $MLMACHINES_FILE | sort | uniq | sh $UTIL_SCRIPTS/get_full_name.sh |    \
                    awk -v conf=$CONF_DIR                                       \
                        -v output="metaproviders"                               \
                        -f $BSMON_CONFIG/sendConfigFile.awk

    sleep 3
    
    # copy the monitoring metadata provider on top of the blobseer metadata provider
    mv $BLOBSEER_HOME/provider/sdht $BLOBSEER_HOME/provider/sdht.init
    cp $BSMON_SUPPORT_HOME/provider/sdht $BLOBSEER_HOME/provider/sdht
fi



if [ "$MONITOR_PROVIDER_MANAGER" == "true" ] 
then
	#create files with the list of nodes corresponding to each configuration file
    cat $CONF_DIR/pmanager.txt | sort | uniq |                                     \
        awk -f $BSMON_CONFIG/splitNodes.awk                                     \
        -v lines="`cat $G5K_DEPLOY_HOME/conf/MLmachines.txt | sort | uniq | wc -l`" \
        -v output="pmanager"
        
        
    # copy the configuration file belonging to each node on its local disk
    cat $MLMACHINES_FILE | sort | uniq | sh $UTIL_SCRIPTS/get_full_name.sh |    \
                    awk -v conf=$CONF_DIR                                       \
                        -v output="pmanager"                                    \
                        -f $BSMON_CONFIG/sendConfigFile.awk

    sleep 3
    
    # copy the monitoring provider manager on top of the blobseer provider manager
    mv $BLOBSEER_HOME/pmanager/pmanager $BLOBSEER_HOME/pmanager/pmanager.init
    cp $BSMON_SUPPORT_HOME/pmanager/pmanager $BLOBSEER_HOME/pmanager/pmanager
fi



if [ "$MONITOR_VERSION_MANAGER" == "true" ] 
then
    #create the ApMon configuration file for the vmanager log parser
    cat $MLMACHINES_FILE | sort | uniq | tail -n 1 | sh $UTIL_SCRIPTS/get_full_name.sh |  awk '
				{ 
				FILE = "monitor-vman.conf"
				command = "cp monitor.conf " FILE
				system(command)
				print $0 >> FILE
				}' 
    cat $MLMACHINES_FILE | sort | uniq | head -n $DUPLICATES >> $CONF_DIR/monitor-vman.conf
fi


