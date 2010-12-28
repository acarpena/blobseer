
if [ !  $#  -eq 2 ]; then
	echo Usage: $0 providers_no metadata_providers_no
	exit
fi

# get the list of nodes
cat nodes | uniq > nodes1

#get the total number of nodes
nodes_no=`wc -l nodes1`
nodes_no=`expr "$nodes_no" : '\([0-9]*\)'`
echo "Nodes number: $nodes_no"

# the first node is the Version Manager
vmgr=`sed -n '1p' nodes1`

# the second node is the Provider Manager
pmgr=`sed -n '2p' nodes1`


#the last pno nodes are the providers
`tail -n $1 nodes1 > providers`

# dhtno metadata providers
x=$(($1 + $2))
`tail -n $x nodes1 | head -n $2 > dhts`

# clientno client nodes
client_no=$(((($nodes_no - 2)) - $x))
`tail -n $(($x + $client_no)) nodes1 | head -n $client_no > clients`


################
# deploy blobseer  #
################

./blobseer-deploy.py -v $vmgr -m $pmgr -d dhts -p providers -l blobseer.cfg
