BEGIN { nr=0 }
{
    nr = nr + 1;
    FILE = "monitor-" nr ".conf"
    NODES = output "-" nr ".txt" 

    #clean = "taktuk -s -d -1 -c \"$CONNECTOR $SSH_OPT \"  -f " NODES " broadcast exec [ rm -rf ~/.ssh/known_hosts ] > /dev/null"
    #system(clean)
    command = "taktuk -s -c \"$CONNECTOR $SSH_OPT \"  -f " NODES " broadcast exec [ cp " conf "/" FILE " $LOCAL_DIR/monitor.conf ] > /dev/null"
    system(command)
}