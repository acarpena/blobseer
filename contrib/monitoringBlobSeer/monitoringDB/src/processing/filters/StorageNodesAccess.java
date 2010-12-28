package processing.filters;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.Serializable;
import java.net.InetAddress;
import java.util.HashMap;
import java.util.Vector;
import processing.NodeCommand;
import util.ByteArrayUtil;
import util.NodeInfo;

import nio.client.NioClient;

import data.MonitorData;

import lia.Monitor.monitor.AppConfig;
import lia.Monitor.monitor.eResult;

/** filter class for MonALISA
 * 
 * @author Michi
 *
 */
public class StorageNodesAccess 
{	
	private HashMap<NodeInfo, NioClient> nio_clients; //the connections with nodes that have already been initialized
	private final int PORT;
	  
    private String nodeFile; //the file with processing nodes
    private Vector<NodeInfo> nodes;
    private HashMap<String, NodeInfo> client_nodes; //the association between a client and a node
    int node_index;
     	
	public StorageNodesAccess() 
	{
        nio_clients = new HashMap<NodeInfo, NioClient>();
        client_nodes = new HashMap<String, NodeInfo>();
        
        PORT = Integer.parseInt(AppConfig.getProperty("BSMonFilter.Port"));
        nodeFile = AppConfig.getProperty("BSMonFilter.nodeFile");
        //read the list of nodes
        readNodes(nodeFile);
        node_index = 0;
	}

	private void readNodes(String fileName) 
	{
		nodes = new Vector<NodeInfo>();
		try 
		{
			//use buffering, reading one line at a time
			//FileReader always assumes default encoding is OK!
			BufferedReader input =  new BufferedReader(new FileReader(fileName));
			try {
				String line = null; //not declared within while loop
				
				while (( line = input.readLine()) != null)
				{
					String []str = line.split(" ");
					NodeInfo node_info;
					if (str.length >= 2)
					{
						node_info = new NodeInfo(str[0], Integer.parseInt(str[1]));
					}
					else
					{
						node_info = new NodeInfo(str[0], PORT);
					}
					nodes.add(node_info);
				}
			}
			finally {
				input.close();
			}
		}
		catch (IOException ex)
		{
			ex.printStackTrace();
		}
	}
	
//	@Override
//	public Object expressResults() //called periodically
//	{
//		//the vector has to be emptied after sending the results
//		Iterator<String> it = monitor_data.keySet().iterator();
//		
//		System.out.println(monitor_data.toString());
//		while (it.hasNext())
//		{ 
//			MonitorData data = monitor_data.get(it.next());
//			NodeInfo client_name = next_node_index(data.clientID);
//			
//			sendMonitoredData(data, client_name);
//		}
//		monitor_data.clear();
//		return null;
//	}
		
	protected void sendMonitoredData(Serializable data, NodeInfo client_name)
	{
		System.out.println("in send data: client info"+client_name.getNodeName()+client_name.getPort());
		
		if (nio_clients.containsKey(client_name)) //if the client exists just send the data
		{
			System.out.println("client exists..trying to send..");
			NioClient client = nio_clients.get(client_name);
			NodeCommand command = new NodeCommand(NodeCommand.PROCESS_MONITOR_DATA, data);
			//serialize &send
			try
			{
				byte[] databytes = ByteArrayUtil.getBytes(command);
				byte[] len = ByteArrayUtil.intToBytes(databytes.length);

				//System.out.println("sending serialized: "+ bytes);
				//System.out.println("sending desserialized: "+ ByteArrayUtil.deserialize(bytes));
				byte[] bytes = new byte[databytes.length + len.length];
				System.arraycopy(len, 0, bytes, 0, len.length);
				System.arraycopy(databytes, 0, bytes, len.length, databytes.length);

				client.send(bytes,null);
			} catch (IOException e)
			{
				System.out.println("exception in send: "+e.toString());
				nio_clients.remove(client_name); //remove in case the connection is no longer active and has to be reinitialized
				e.printStackTrace();
			}
		}
		if (!nio_clients.containsKey(client_name))  //if it doesn't exist initialize the connection with the node and then send
		{
			System.out.println("client does not exist");
			try
			{
				NioClient client = new NioClient(InetAddress.getByName(client_name.getNodeName()), client_name.getPort());
				Thread t = new Thread(client);
				//t.setDaemon(true);
				t.start();
				nio_clients.put(client_name, client); //put in the map
				//build command
				NodeCommand command = new NodeCommand(NodeCommand.PROCESS_MONITOR_DATA, data);
				//byte[] bytes = ByteArrayUtil.getBytes(command);
				//System.out.println("sending serialized: "+ bytes);
				//System.out.println("sending desserialized: "+ ByteArrayUtil.deserialize(bytes));
				
                byte[] databytes = ByteArrayUtil.getBytes(command);
                byte[] len = ByteArrayUtil.intToBytes(databytes.length);
                    
                byte[] bytes = new byte[databytes.length + len.length];
                System.arraycopy(len, 0, bytes, 0, len.length);
                System.arraycopy(databytes, 0, bytes, len.length, databytes.length);


				//serialize &send
				client.send(bytes,null);
			} 
			catch (IOException e)
			{
				System.out.println("exception in send - client does not exist ");
				e.printStackTrace();
			}
		}
	}
	
	
	/**
	 * Returns the name of the node to which this data should be sent
	 * @param data
	 * @return
	 */
	protected NodeInfo getNodeName(MonitorData data)
	{		
		if (client_nodes.containsKey(data.clientID))  //this client already has a node assigned
		{
			return client_nodes.get(data.clientID);
		}
		else   //no node, assign one
		{
			NodeInfo node_info = next_node_index(data.clientID);
			client_nodes.put(data.clientID, node_info);
			return node_info;
		}
	}
	
	protected NodeInfo next_node_index(String clientID) //get the next node to which we send data 
	{
	//we add the numbers contained in the ip and select the node in the list corresponding to the generated index
		int node_no = nodes.size();
		String client = clientID;
		int t = 0;
		try
		{
			String s1 = InetAddress.getByName(client).getHostAddress().toString();
			String []str = s1.split("\\.");
			for(String s:str)	
			{
				t+= Integer.parseInt(s);
			}
		}
		catch (Exception e)
		{
			t = 0;
			e.printStackTrace();
		}
		return nodes.get(t % node_no);
		/*node_index = (node_index % nodes.size());
		int result = node_index;
		node_index++;
		return (NodeInfo)nodes.get(result);*/
	}
	         
    
    @SuppressWarnings("unused")
	private String formateResult(eResult er) 
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

}


