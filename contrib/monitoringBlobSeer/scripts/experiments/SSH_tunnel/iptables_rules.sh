# for the 1st Lookup Service - monalisa.cern.ch
iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 80  -j DNAT --to-destination 127.0.0.1:80
iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 8765  -j DNAT --to-destination 127.0.0.1:8765
iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 4160  -j DNAT --to-destination 127.0.0.1:4160


# for proxy server monalisa.cern.ch
iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 6002  -j DNAT --to-destination 127.0.0.1:6002
iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 6001  -j DNAT --to-destination 127.0.0.1:6001
iptables -t nat -A OUTPUT -d monalisa.cern.ch -p tcp --dport 6003  -j DNAT --to-destination 127.0.0.1:6003



# for the 2nd Lookup Service - monalisa.cacr.caltech.edu
iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 80  -j DNAT --to-destination 127.0.0.1:8080
iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 8765  -j DNAT --to-destination 127.0.0.1:8760
iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 4160  -j DNAT --to-destination 127.0.0.1:8160



# for proxy server monalisa.cacr.caltech.edu
iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 6002  -j DNAT --to-destination 127.0.0.1:8002
iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 6001  -j DNAT --to-destination 127.0.0.1:8001
iptables -t nat -A OUTPUT -d monalisa.cacr.caltech.edu -p tcp --dport 6003  -j DNAT --to-destination 127.0.0.1:8003


# for proxy server monalisa.caltech.edu
iptables -t nat -A OUTPUT -d monalisa.caltech.edu -p tcp --dport 6002  -j DNAT --to-destination 127.0.0.1:5002
iptables -t nat -A OUTPUT -d monalisa.caltech.edu -p tcp --dport 6001  -j DNAT --to-destination 127.0.0.1:5001
iptables -t nat -A OUTPUT -d monalisa.caltech.edu -p tcp --dport 6003  -j DNAT --to-destination 127.0.0.1:5003


# for proxy server monalisa2.cern.ch
iptables -t nat -A OUTPUT -d monalisa2.cern.ch -p tcp --dport 6002  -j DNAT --to-destination 127.0.0.1:5102
iptables -t nat -A OUTPUT -d monalisa2.cern.ch -p tcp --dport 6001  -j DNAT --to-destination 127.0.0.1:5101
iptables -t nat -A OUTPUT -d monalisa2.cern.ch -p tcp --dport 6003  -j DNAT --to-destination 127.0.0.1:5103


# for proxy server monalisa-ul.caltech.edu
iptables -t nat -A OUTPUT -d monalisa-ul.caltech.edu -p tcp --dport 6002  -j DNAT --to-destination 127.0.0.1:5202
iptables -t nat -A OUTPUT -d monalisa-ul.caltech.edu -p tcp --dport 6001  -j DNAT --to-destination 127.0.0.1:5201
iptables -t nat -A OUTPUT -d monalisa-ul.caltech.edu -p tcp --dport 6003  -j DNAT --to-destination 127.0.0.1:5203
