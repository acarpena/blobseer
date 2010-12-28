#!/usr/bin/env python

# This script launches BlobSeer on all configured machines
import threading
import getopt
import sys
import os
import subprocess

# What ssh like command are we using? (maybe oarsh :) )
SSH="oarsh"
SCP="oarcp"
TMP_CFG_FILE="./blobseer-deploy.cfg"
REMOTE_CFG_FILE="/tmp/blobseer.cfg"
KEY="/tmp/oargrid/oargrid_ssh_key_jcai_19760"
GRID_JOB="19760"
CLUSTER_JOBS="./cluster_job_ids"



def usage():
    print "%s [-v, --vmgr <hostname>] [-m, --pmgr <hostname>] [-d, --dht <dht.txt>] [-p, --providers <providers.txt>] [ [-l, --launch] | [-k, --kill] | [-s, --status] ]\n" % sys.argv[0]

def read_hosts(file_name):
    f = open(file_name, "r")
    hosts = map(lambda x: x.strip(), filter(lambda x: x != "\n", f.readlines()))
    f.close()
    return hosts


def get_env(name):
    try:
        return os.environ[name]
    except KeyError:
        return "."
    
def gen_launch(path, process):
    out = "/tmp/blobseer/" + process
    return "\"rm -rf " + out + "; " + \
        "mkdir -p " + out + "; " + \
        "env CLASSPATH=" + get_env("CLASSPATH") + " " + \
        "LD_LIBRARY_PATH=" + get_env("LD_LIBRARY_PATH") + " " + \
        get_env("BLOBSEER_HOME") + "/" + path + "/" + process + " " + \
        REMOTE_CFG_FILE + " >" + out + "/" + process + ".stdout 2>" + out + "/" + process + ".stderr&\""

def gen_kill(path, process):
    return "\"killall " + process + " || pgrep "+ process +" | xargs kill -9  || echo could not kill " + process + "\""

def gen_status(path, process):
    return "\"test `ps aux | grep " + path + "/" + process + " | grep -v grep | wc -l` -eq 1 || echo no " + process + " running\""

class CommandLauncher(threading.Thread):
    def __init__(self, n, cmd):
        threading.Thread.__init__(self)
        self.node = n
	splitted = self.node.split('-')
        JOB_ID = cluster_jobs.get(splitted[0],None)

	self.cmd ="OAR_JOB_ID=" +str(JOB_ID)+ " " + SSH + " -i" +KEY + " " + self.node + " " + cmd
            
    def run(self):
        if (needcfg):
	    splitted = self.node.split('-') 
	    JOB_ID = cluster_jobs.get(splitted[0],None)
	    print "OAR_JOB_ID=",JOB_ID, " " , self.node
            p = subprocess.Popen("OAR_JOB_ID=" +str(JOB_ID)+ " " + SCP + " -i" +KEY +" " + TMP_CFG_FILE + " " + self.node + ":" + REMOTE_CFG_FILE + " &>/dev/null", shell=True)
            if p.wait() != 0:
                print "[%s]: could not copy configuration file job id:%s , cluster:%d  " % (self.node,JOB_ID,splitted[0])
                return
        p = subprocess.Popen(self.cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        (cout, cerr) = p.communicate()
        if cout.strip() != "":
            print "[%s]: %s" % (self.node, cout.strip())
            
(opts, args) = getopt.getopt(sys.argv[1:], "v:m:p:d:l:ks", ["vmgr=", "pmgr=", "providers=", "dht=", "launch=", "kill", "status"])
op = ""
machines = ""
template = ""
vmgr = ""
pmgr = ""
providers = []
dht = []
gencmd = gen_launch
needcfg = False
for (o, a) in opts:
    if o in ("-h", "--help"):
        usage()
        sys.exit(2)
    elif o in ("-v", "--vmgr"):
        vmgr = a
    elif o in ("-m", "--pmgr"):
        pmgr = a
    elif o in ("-p", "--providers"):
        providers = read_hosts(a)
    elif o in ("-d", "--dht"):
        dht = read_hosts(a)
    elif o in ("-l", "--launch"):
        f = open(a, "r")
        template = f.readlines()
        f.close()
        needcfg = True
    elif o in ("-k", "--kill"):
        gencmd = gen_kill
    elif o in ("-s", "--status"):
        gencmd = gen_status

if vmgr == "" or pmgr == "" or providers == [] or dht == []:
    usage()
    sys.exit(1)


#creating:
cluster_jobs = {'gdx':244416, 'chinqchint':960224, 'griffon':218853}



if (needcfg):
    gateways = ",\n\t\t".join(map(lambda x: "\"" + x + "\"", dht))
    f = open(TMP_CFG_FILE, "w")
    f.writelines(map(lambda x: x.replace("${gateways}", gateways).
                     replace("${vmanager}", "\"" + vmgr + "\"").
                     replace("${pmanager}", "\"" + pmgr + "\""), template))
    f.close()


#print "dht = ", dht
#print "providers = ", providers
workers = []
workers.append(CommandLauncher(vmgr, gencmd("vmanager", "vmanager")))
workers.append(CommandLauncher(pmgr, gencmd("pmanager", "pmanager")))
for p in providers:
    workers.append(CommandLauncher(p, gencmd("provider", "provider")))
for d in dht:
    workers.append(CommandLauncher(d, gencmd("provider", "sdht")))
    
# Now launching workers
for w in workers:
    w.start()

# Wait for all workers to finish
for w in workers:
    w.join()

# Do some cleanup
if (needcfg):
    os.remove(TMP_CFG_FILE)

print "All done!"