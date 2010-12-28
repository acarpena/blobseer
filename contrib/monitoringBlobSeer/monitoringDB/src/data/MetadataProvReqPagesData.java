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
public class MetadataProvReqPagesData extends MonitorData
{
    /**
	 * 
	 */
	private static final long serialVersionUID = 534270918834772712L;
    public String page_id;
    public String node;
    public String reqTimestamp;
    
    public MetadataProvReqPagesData()
    {
    	clientID = new String();
    	timestamp = new String();
    	page_id = new String();
    	node = new String();
    	reqTimestamp =  new String();
    }
    
    public MetadataProvReqPagesData(String f, String n, String ns, String cid, String tm)
    {
        super(cid);
 
        node = new String(n);
        page_id = new String(ns);
        reqTimestamp =  new String(tm);
    }

  
    public MetadataProvReqPagesData(String f, String n, String[] data)
    {
        super(data[2]);
        node = n;
        
        // the monitored parameter:
        // page id----request timestamp (to filter duplicates sent to different ML farms)---client id
        page_id = data[0];   
        reqTimestamp =  data[1];
    }
    
    public MetadataProvReqPagesData(MetadataProvReqPagesData data)
    {
        super(data.timestamp, data.clientID);
        
        node = data.node;
        page_id = data.page_id;
        reqTimestamp =  data.reqTimestamp;
    }

	@Override
	public int getMonDataType()
	{
		return MonitorData.METADATA_PROV_PAGEID;
	}

}
