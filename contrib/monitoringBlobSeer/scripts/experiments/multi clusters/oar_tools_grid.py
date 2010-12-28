from subprocess import Popen

ssh_key=""
slice_name=""

accepted_codes = [0, 143]
max_simult_conn = 25

def ssh_extra_ops(timeout=3) :

    ops = ["-o", "ConnectTimeout="+str(timeout)+"", "-o", "StrictHostkeyChecking=no"]
    return ops

# --------------------------------------------------------------------------------

def read_nodes(file_name) :

    res = []
    file = open(file_name, "r")

    for line in file :
        if line != "\n" :
            res.append(line[0:-1])

    return res

# --------------------------------------------------------------------------------

#def copy_files(node_list, files, echo=True, timeout=3) :
    
#    remaining = list(node_list)

#    while len(remaining) > 0 :
#        current = []
            
#        while (len(remaining) > 0) and (len(current) < max_simult_conn) :
#            current.append(remaining.pop())
            
#        print "-"*50
        
#        proc_list = []
#        for node in current :
#            scp_command = ["scp", "-i", ssh_key] + ssh_extra_ops(timeout) + files + [slice_name+"@"+node+":"]
#            if echo :
#                p = Popen(scp_command)
#            else :
#                output = open("/dev/null", "w")
#                p = Popen(scp_command, stdout=output)
#            proc_list.append(p)
#        for proc in proc_list:
#            proc.wait()

# --------------------------------------------------------------------------------

#def sync_files(node_list, files, echo=True, timeout=3) :
    
#    remaining = list(node_list)

#    while len(remaining) > 0 :
#        current = []
            
#        while (len(remaining) > 0) and (len(current) < max_simult_conn) :
#            current.append(remaining.pop())

#        print "-"*50
        
#        proc_list = []
#        for node in current :
#            ssh_for_rsync = ("ssh -l "+slice_name+" -i "+ssh_key+
#                             " -o ConnectTimeout="+str(timeout)+" -o StrictHostkeyChecking=no")
#            rsync_command = (["rsync", "-avz", "--rsh="+ssh_for_rsync, "--delete"] +
#                             files + [slice_name+"@"+node+":"])
#            if echo :
#                p = Popen(rsync_command)
#            else :
#                output = open("/dev/null", "w")
#                p = Popen(rsync_command, stdout=output)
#            proc_list.append(p)
#        for proc in proc_list:
#            proc.wait()

# --------------------------------------------------------------------------------

def execute_command(node_list, command, wait=True, echo=True, timeout=3) :
    
    proc_list = []
    for node in node_list :
        #ssh_command = ["ssh", "-i", ssh_key]+ssh_extra_ops(timeout)+[slice_name+"@"+node]+command
	
	ssh_command = ["OAR_JOB_ID=960224 oarsh -i /tmp/oargrid/oargrid_ssh_key_jcai_19760", node]+command
        print ssh_command
	if echo :
            p = Popen(ssh_command)
        else :
            output = open("/dev/null", "w")
            p = Popen(ssh_command, stdout=output)
        proc_list.append(p)
        
    if wait :
        pos = 0
        for proc in proc_list:
            res = proc.wait()
            if accepted_codes.count(res) == 0 :
                print "ERROR: Command execution failed in "+node_list[pos]+" (return code "+str(res)+")"
            pos = pos + 1
            
# --------------------------------------------------------------------------------

