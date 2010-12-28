package processing.filters;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.Vector;
import processing.NodeCommand;
import util.ByteArrayUtil;
import util.NodeInfo;

import nio.client.NioClient;

import data.MonitorData;
import data.ProviderData;
import data.VManagerData;

import lia.Monitor.Filters.GenericMLFilter;
import lia.Monitor.monitor.AppConfig;
import lia.Monitor.monitor.eResult;
import lia.Monitor.monitor.monPredicate;

/** filter class for MonALISA
 * 
 * @author Michi
 *
 */
@SuppressWarnings("serial")
public class BSMonFilter extends GenericMLFilter
{
	private final static String Name = "BSMonFilter";
	private HashMap<String,MonitorData> monitor_data; //each client has a vector associated with related monitoring data
	
	private HashMap<NodeInfo, NioClient> nio_clients; //the connexions with nodes that have already been initialized
	private final int PORT;
	private static long TIMEOUT;
	  
    private monPredicate[] predicates = null;
    private Properties properties;
    private String nodeFile; //the file with processing nodes
    private Vector<NodeInfo> nodes;
    private HashMap<String, NodeInfo> client_nodes; //the association between a client and a node
    int node_index;
     	
	public BSMonFilter( String farmName ) 
	{
        super( farmName );
        monitor_data = new HashMap<String, MonitorData>();
                
        nio_clients = new HashMap<NodeInfo, NioClient>();
        client_nodes = new HashMap<String, NodeInfo>();
        
        String localPropsFileName = AppConfig.getProperty("BSMonFilter.ConfigFile", null); //the file containing predicates for the filter
        loadConfig(localPropsFileName);
        PORT = Integer.parseInt(AppConfig.getProperty("BSMonFilter.Port"));
        nodeFile = AppConfig.getProperty("BSMonFilter.nodeFile");
        //read the list of nodes
        readNodes(nodeFile);
        node_index = 0;
        try
        {
        	TIMEOUT = Long.parseLong(AppConfig.getProperty("BSMonFilter.filterTimeout"));
        	TIMEOUT *= 1000; //turn it in miliseconds
        }
        catch (Exception e)
        {
        	TIMEOUT = 10 * 1000;
        }
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
	
	@Override
	public Object expressResults() //called paeriodically
	{
		//the vector has to be emptied after sending the results
		Iterator<String> it = monitor_data.keySet().iterator();
		
		System.out.println(monitor_data.toString());
		while (it.hasNext())
		{ 
			MonitorData data = monitor_data.get(it.next());
			NodeInfo client_name = next_node_index(data.clientID);
			System.out.println("client info"+client_name.getNodeName()+client_name.getPort());
			
			if (nio_clients.containsKey(client_name)) //if the client exists just send the data
			{
				NioClient client = nio_clients.get(client_name);
				NodeCommand command = new NodeCommand(NodeCommand.PROCESS_MONITOR_DATA, data);
				//serialize &send
				try
				{
					client.send(ByteArrayUtil.getBytes(command),null);
				} catch (IOException e)
				{
					nio_clients.remove(client_name); //remove in case the connection is no longer active and has to be reintialized
					e.printStackTrace();
				}
			}
			if (!nio_clients.containsKey(client_name))  //if it doesn't exist initialize the connection with the node and then send
			{
				try
				{
					NioClient client = new NioClient(InetAddress.getByName(client_name.getNodeName()), client_name.getPort());
					Thread t = new Thread(client);
					//t.setDaemon(true);
					t.start();
					nio_clients.put(client_name, client); //put in the map
					//build command
					NodeCommand command = new NodeCommand(NodeCommand.PROCESS_MONITOR_DATA, data);
					//serialize &send
					client.send(ByteArrayUtil.getBytes(command),null);
				} 
				catch (IOException e)
				{
					e.printStackTrace();
				}
			}
		}
		monitor_data.clear();
		return null;
	}
		
	/**
	 * Returns the name of the node to which this data should be sent
	 * @param data
	 * @return
	 */
	private NodeInfo getNodeName(MonitorData data)
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
	
	private NodeInfo next_node_index(String clientID) //get the next node to which we send data 
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
	
	private MonitorData parseResult(eResult r)
	{
		 if (r.ClusterName.equals("Blob_IO"))
         {
         	return parseProviderData(r);
         }
            
         if (r.ClusterName.equals("VManager"))
         {
        	 return parseVmanagerData(r);
         }
		return null;
	}
	
	private ProviderData parseProviderData(eResult r)
	{
		String sParam = r.param_name[0]; 
        
        String[] IO_arr= ((String)r.param[0]).split("#");
        
        if (sParam.equals("provider_write"))
        {
        	 ProviderData prov = new ProviderData(r.FarmName, r.NodeName, IO_arr);
        	 return prov;
        }
        return null;
        
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
    
	private VManagerData parseVmanagerData(eResult r)
	{
		String sParam = r.param_name[0];
        
        String[] IO_arr=((String)r.param[0]).split("#");
        
        if (sParam.equals("blobWrite"))
        {
        	VManagerData vman = new VManagerData(IO_arr);
        	return vman;
        }
        return null;
	}
	
	private void loadConfig(String localPropsFile) //load the predicates from which we select data
	{
		properties = new Properties();
        FileInputStream fis = null;
        try 
        {
            fis = new FileInputStream(localPropsFile);
            properties.load(new BufferedInputStream(fis));
        }
        catch(IOException e) 
        {
           System.out.println(" Got exception loading local conf "+ e.getLocalizedMessage());
            return;
        } 
        finally 
        {
            try 
            {
                fis.close();
            }
            catch(Throwable ignore){}
        }
        
		 String PredS = null;
         try 
         {
             PredS = properties.getProperty("PREDICATES", null);
         }
         catch(Throwable t) 
         {
             System.out.println(" [ " + Name + " ] Got exception reading PREDICATES property"+ t.getLocalizedMessage());
             PredS = null;
         }
         
         if(PredS != null) 
         {
             PredS = PredS.trim();
             if(PredS.length() > 0) 
             {
                 predicates = parsePredicates(PredS);
             } else 
             {
                 predicates = null;
             }
         } 
         else 
         {
             predicates = null;
         }
         
         if(predicates == null) 
         {
             System.out.println( " [ " + Name + " ] NO Predicates will filter the flow");
         } 
         else 
         {
             System.out.println(" [ " + Name + " ] will use the following monPredicates[] to filter the flow:\n " + PredS + "\n");
         }
	}

	private monPredicate[] parsePredicates(String predicatesS) 
	{
        String[] pPreds = predicatesS.split("(\\s)*;(\\s)*");
        System.out.println( "\n\n [ " + Name + " ] Trying to parse predicates: " + predicatesS + " \n\n");
       
        ArrayList<monPredicate> alPreds = new ArrayList<monPredicate>();
        for(int i=0; i<pPreds.length; i++) 
        {
            try 
            {
                alPreds.add(toPred(pPreds[i]));
            }
            catch(Throwable t) 
            {
                System.out.println( " [ " + Name + " ] Got exception parsing predicate [ " + pPreds[i] + " ]"+ t.getLocalizedMessage());
            }
        }
        
        if(alPreds.size() == 0) 
        {
            return null;
        }
        return (monPredicate[])alPreds.toArray(new monPredicate[alPreds.size()]);
    }
    
    public static final monPredicate toPred(final String s) 
    {
        if (s == null || s.trim().length() <= 0)
            return null;

        final StringTokenizer st = new StringTokenizer(s, "/");

        if (!st.hasMoreTokens())
            return null;

        final String sFarm = st.hasMoreTokens() ? st.nextToken() : "*";
        final String sCluster = st.hasMoreTokens() ? st.nextToken() : "*";
        final String sNode = st.hasMoreTokens() ? st.nextToken() : "*";

        final String sTime1 = st.hasMoreTokens() ? st.nextToken() : "-1";
        final String sTime2 = st.hasMoreTokens() ? st.nextToken() : "-1";
        
        // The default format is F/C/N/time1/time2/P
        // but it accepts two alternatives:
        // F/C/N/time1/P    with time2 = -1
        // F/C/N/P          with time1 = time2 = -1
        // the alternatives are chosen based on the existence of the other parameters and on the fact that they are numeric or not
        String sFunc = st.hasMoreTokens() ? st.nextToken() : null;

        long lTime1, lTime2;

        try {
            lTime1 = Long.parseLong(sTime1);

            try {
                lTime2 = Long.parseLong(sTime2);
            } catch (Exception e) {
                lTime2 = -1;
                sFunc = sTime2;
            }
        } catch (Exception e) {
            lTime1 = lTime2 = -1;
            sFunc = sTime1;
        }

        String[] vsFunc;
        if (sFunc == null) {
            vsFunc = null;
        } else {
            final Vector<String> v = new Vector<String>();
            final StringTokenizer st2 = new StringTokenizer(sFunc, "|");

            while (st2.hasMoreTokens()) {
                v.add(st2.nextToken().trim());
            }

            if (v.size() > 0) {
                vsFunc = new String[v.size()];
                for (int j = 0; j < v.size(); j++)
                    vsFunc[j] = (String) v.get(j);
            } else {
                vsFunc = null;
            }
        }

        return new monPredicate(sFarm, sCluster, sNode, lTime1, lTime2, vsFunc, null);
    }
    
	@Override
	public monPredicate[] getFilterPred()
	{
		return predicates;
	}

	@Override
	public String getName()
	{
		return Name;
	}

	@Override
	public long getSleepTime()
	{
		return TIMEOUT; //in miliseconds
	}

	@Override
	public void notifyResult(Object o) //called for every match inside the filter
	{
		if (o == null) return;
		 if ( o instanceof Vector<?> ) {
             Vector<?> v = (Vector<?>) o;
             for ( int i = 0; i < v.size(); i++ ) 
             {
                 notifyResult( v.elementAt( i ) );
             }

         } 
		else if (o instanceof eResult)
		{
			eResult r = (eResult) o;
			MonitorData data  = parseResult(r);
			String key = null;
		
			if (data.getMonDataType().equals(MonitorData.PROVIDER_STRING))
			{
				ProviderData provider_data = (ProviderData) data;
				System.out.println("provider data");
				key = hashKey("p",provider_data.blob_id, provider_data.watermark, provider_data.page_size);
				System.out.println("monitor data client id ="+provider_data.clientID+" blob id ="+provider_data.blob_id+" farm ="+provider_data.farm+"node = "+provider_data.node+" page_index="+provider_data.page_index+" page number ="+provider_data.page_number+" page size ="+provider_data.page_size+" watermark = "+provider_data.watermark+" timestamp ="+provider_data.timestamp);
				if (monitor_data.containsKey(key)) //aggregate data if it is the case
				{
					MonitorData data1 = monitor_data.get(key);
					ProviderData prov_data1= (ProviderData)data1;
					//ce e cu page index
					prov_data1.page_index = "0";
					prov_data1.page_number++ ; //another page of the same write
					monitor_data.put(key, data1);
					System.out.println("new page_number"+prov_data1.page_number);
					System.out.println("monitor data client id ="+prov_data1.clientID+" blob id ="+prov_data1.blob_id+" farm ="+prov_data1.farm+"node = "+prov_data1.node+" page_index="+prov_data1.page_index+" page number ="+prov_data1.page_number+" page size ="+prov_data1.page_size+" watermark = "+prov_data1.watermark+" timestamp ="+prov_data1.timestamp);
				}
				else
				{
					System.out.println("intrare noua");
					monitor_data.put(key, data);
				}
			}
			else
			{
				if (data.getMonDataType().equals(MonitorData.VMANAGER_STRING))
				{
					VManagerData vman_data= (VManagerData) data;
					key = hashKey("v", vman_data.blob_id, vman_data.watermark, vman_data.page_size);
					System.out.println("hash key = "+key);
					System.out.println("vmanager data blob id="+vman_data.blob_id+" client id= "+vman_data.clientID+" page size="+vman_data.page_size+" timestamp="+vman_data.timestamp+" version= "+vman_data.version+" watermark="+vman_data.watermark+" write offset="+vman_data.writeOffset+" write size="+vman_data.writeSize);
					if (!monitor_data.containsKey(key)) //put data if no entry was there
					{
						monitor_data.put(key, data);
						System.out.println("inserat vmanager data");
					}
				}
			}
			
		}
	}
	
	private String hashKey(String s,String blob_id, String watermark, String page_size)
	{
		String result = new String(s + " " +blob_id+" "+watermark+" "+page_size);
		return result;
	}

}
