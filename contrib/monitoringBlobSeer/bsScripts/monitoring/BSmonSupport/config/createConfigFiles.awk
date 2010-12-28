BEGIN { nr=0 }
{ 
    nr = nr + 1;
    FILE = "monitor-" nr ".conf"
    command = "cp monitor.conf " FILE
    system(command)

    command="head -n " nr " " mlmachines " | tail -n " duplicates
    dd = duplicates + 1;
    if (nr < dd)
    {
        command="head -n " dd " " mlmachines " | tail -n " duplicates
    }
    while (command |getline) {print >> FILE}
    close(command)
    #command | getline result
    #print $0 >> FILE
}