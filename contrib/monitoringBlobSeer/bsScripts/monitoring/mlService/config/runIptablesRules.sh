
#usage: sh runIptablesRules.sh MLmachines_file

MLnodes=$1

echo "Executing the iptables script..."
taktuk -l root -f $MLnodes broadcast exec [ sh mlScripts/executeIptablesRules.sh ]
