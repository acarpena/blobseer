//
//  MetadataProvNodesData.java
//
//  Created by Alexandra Carpen-Amarie on 5/5/09.
//
package data;

/**
 * encapsulates the the monitoring information sent by BlobSeer providers
 * 
 */
public class MetadataProvNodesData extends MonitorData
{
    /**
	 * 
	 */
	private static final long serialVersionUID = 534270918834772712L;
    public String number_of_nodes;
    public String node;
    public String reqTimestamp;
    
    public MetadataProvNodesData()
    {
    	clientID = new String();
    	timestamp = new String();
    	number_of_nodes = new String();
    	node = new String();
    	reqTimestamp =  new String();
    }
    
    public MetadataProvNodesData(String f, String n, String ns, String cid, String tm)
    {
        super(cid);
 
        node = new String(n);
        number_of_nodes = new String(ns);
        reqTimestamp =  new String(tm);
    }

  
    public MetadataProvNodesData(String f, String n, String[] data)
    {
        super(data[2]);
        node = n;
        
        // the monitored parameter:
        // number of requested nodes----request timestamp (to filter duplicates sent to different ML farms)---client id
        number_of_nodes = data[0];   
        reqTimestamp =  data[1];
    }
    
    public MetadataProvNodesData(MetadataProvNodesData data)
    {
        super(data.timestamp, data.clientID);
        
        node = data.node;
        number_of_nodes = data.number_of_nodes;
        reqTimestamp =  data.reqTimestamp;
    }

	@Override
	public int getMonDataType()
	{
		return MonitorData.METADATA_PROV_NODES;
	}

}
