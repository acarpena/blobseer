//
//  ProviderData.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 5/5/09.
//
package data;

/**
 * encapsulates the the monitoring information sent by BlobSeer providers
 * 
 */
public class ProvManagerData extends MonitorData
{
    /**
	 * 
	 */
	private static final long serialVersionUID = 534270918834772712L;
	public String prov_id;
    public String request_id;
    public String node;
    
    public ProvManagerData()
    {
    	clientID = new String();
    	timestamp = new String();
    	prov_id = new String();
    	request_id = new String();
    	node = new String();
    }
    
    public ProvManagerData(String f, String n, String pid, String rid, String cid)
    {
        super(cid);
 
        node = new String(n);
        prov_id = new String(pid);
        request_id = new String(rid);
    }

  
    public ProvManagerData(String f, String n, String[] data)
    {
        super(data[2]);
        node = n;
        
        // the monitored parameter:
        // prov_id (hostname), requestId (a random timestamp), clientId
        prov_id = data[0];
        request_id = data [1];        
    }
    
    public ProvManagerData(ProvManagerData pm)
    {
        super(pm.timestamp, pm.clientID);
        
        node = pm.node;
        prov_id = pm.prov_id;
        request_id = pm.request_id; 
    }

	@Override
	public int getMonDataType()
	{
		return MonitorData.PMANAGER;
	}

}
