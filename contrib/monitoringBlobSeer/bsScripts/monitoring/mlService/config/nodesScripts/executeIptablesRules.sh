# apply the iptables rules

#assign ports for the tunnels
# extract a number from the host name
MACHINE_NUMBER=`echo $HOSTNAME | 	\
		sed "s/grid5000//g" |	\
		sed "s/[^0-9]//g"
		`		
if [ ${#MACHINE_NUMBER} = 1 ] ; then
   MACHINE_NUMBER="0$MACHINE_NUMBER"
fi
echo "Used ports for tunnels will be 1${MACHINE_NUMBER}00-1${MACHINE_NUMBER}21"

FIRST_PORT="1${MACHINE_NUMBER}00"

sh iptables_rules.sh $FIRST_PORT