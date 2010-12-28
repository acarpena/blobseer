package processing.filters;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Vector;

import lia.Monitor.monitor.eResult;
import util.NodeInfo;
import data.MonitorData;

public class ProcessMonData {
	
	private StorageNodesAccess storageNodes;
	private HashMap<String,Vector<MonitorData>> monitor_data; //each client has a vector associated with related monitoring data 
	
	public ProcessMonData() 
	{
        monitor_data = new HashMap<String, Vector<MonitorData>>();
        storageNodes = new StorageNodesAccess();
	}
	
	public Object expressResults() //called periodically
	{
		synchronized (monitor_data) 
		{
			//the vector has to be emptied after sending the results
			Iterator<String> it = monitor_data.keySet().iterator();
			System.out.println(monitor_data.toString());
			while (it.hasNext()) {
				Vector<MonitorData> data = monitor_data.get(it.next());
				NodeInfo client_name = storageNodes.next_node_index(data.get(0).clientID);

				storageNodes.sendMonitoredData(data, client_name);
			}
			monitor_data.clear();
		}
		return null;
	}
		
	public String formateResult(eResult er) 
	{
        StringBuffer sb = new StringBuffer();
        sb.append(" --> \t").append(er.NodeName);
        sb.append("\t ").append(er.ClusterName);
        sb.append("\t").append(er.FarmName);
        sb.append("\tTime = ").append(er.time) ;
        if(er.param_name == null) {
            sb.append("    param_name is null");
            if(er.param == null) {
                sb.append("    and param is also null");
            } else {
                sb.append("    and param [ length = ").append(er.param.length).append(" ] is NOT");
                for ( int i=0; i < er.param.length ; i++ ) {
                    sb.append("     * param[ ").append(i).append(" ] = ").append(er.param[i]).append("\t");
                 }
            }
            return sb.toString();
        }
        for ( int i=0; i < er.param_name.length ; i++ ) {
           sb.append("     * ").append(er.param_name[i]).append(" = \t").append(er.param[i]);
        }
        return sb.toString();
    }
    

	public void processData(MonitorData received_param)
	{
		System.out.println("provider data");
		System.out.println("monitor data client id ="+received_param.clientID);

		if (monitor_data.containsKey(received_param.clientID)) //the client is already present in the received data
		{
			Vector<MonitorData> data = monitor_data.get(received_param.clientID);
			data.add(received_param);
		}
		else	// data belong to a new client
		{
			System.out.println("intrare noua");
			Vector<MonitorData> data = new Vector<MonitorData>();
			data.add(received_param);
			monitor_data.put(received_param.clientID, data);
		}

	}

}
