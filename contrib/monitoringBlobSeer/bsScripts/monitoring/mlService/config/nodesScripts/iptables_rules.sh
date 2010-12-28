
if [ $# -ne 1 ] ; then        # if the number of params is not 1
    echo Usage: $0 first_port
    exit 1
fi

PORT=$1


# for the 1st Lookup Service - monalisa.cern.ch

iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 80  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 8765  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 4160  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))


# for proxy server monalisa.cern.ch

iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 6002  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 6001  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 6003  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))



# for the 2nd Lookup Service - monalisa.cacr.caltech.edu

iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 80  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 8765  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 4160  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))


# for proxy server monalisa.cacr.caltech.edu

iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 6002  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 6001  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 6003  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))



# for proxy server monalisa.caltech.edu

iptables -t nat -A OUTPUT -d monalisa.caltech.edu -p tcp --dport 6002  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa.caltech.edu -p tcp --dport 6001  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa.caltech.edu -p tcp --dport 6003  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))


# for proxy server monalisa2.cern.ch

iptables -t nat -A OUTPUT -d monalisa2.cern.ch -p tcp --dport 6002  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa2.cern.ch -p tcp --dport 6001  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa2.cern.ch -p tcp --dport 6003  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))



# for proxy server monalisa-ul.caltech.edu

iptables -t nat -A OUTPUT -d monalisa-ul.caltech.edu -p tcp --dport 6002  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa-ul.caltech.edu -p tcp --dport 6001  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

iptables -t nat -A OUTPUT -d monalisa-ul.caltech.edu -p tcp --dport 6003  -j DNAT --to-destination 127.0.0.1:$PORT
PORT=$(($PORT + 1))

echo $PORT

