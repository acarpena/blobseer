
if [ !  $#  -eq 2 ]; then
	echo Usage: $0 providers_no metadata_providers_no
	exit
fi

# get the list of nodes
cat nodes | sort | uniq > nodes.txt

#get the total number of nodes
nodes_no=`wc -l nodes.txt`
nodes_no=`expr "$nodes_no" : '\([0-9]*\)'`
echo "Nodes number: $nodes_no"

# the first node is the Version Manager
vmgr=`sed -n '1p' nodes.txt`

# the second node is the Provider Manager
pmgr=`sed -n '2p' nodes.txt`


#the last pno nodes are the providers
`tail -n $1 nodes.txt > providers.txt`

# dhtno metadata providers
x=$(($1 + $2))
`tail -n $x nodes.txt | head -n $2 > dhts.txt`

# clientno client nodes
client_no=$(((($nodes_no - 2)) - $x))
`tail -n $(($x + $client_no)) nodes.txt | head -n $client_no > clients.txt`


################
# deploy blobseer  #
################

./blobseer-deploy.py -v $vmgr -m $pmgr -d dhts.txt -p providers.txt -l blobseer.cfg
