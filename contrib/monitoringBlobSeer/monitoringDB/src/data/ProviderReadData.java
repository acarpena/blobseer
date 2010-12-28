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
public class ProviderReadData extends MonitorData
{
    /**
	 * 
	 */
	private static final long serialVersionUID = 534270918834772712L;
	public String page_id;
	public String blob_id;
    public String watermark;
    public String farm;
    public String node;
    public String page_size;
    public String providerTimestamp;
    
    // write message
    public ProviderReadData()
    {
    	page_id = new String();
    	clientID = new String();
    	timestamp = new String();
    	blob_id = new String();
    	watermark = new String();
    	farm = new String();
    	node = new String();
    	page_size = new String();
    	providerTimestamp = new String();
    }
    
    public ProviderReadData(String f, String n, String pid, String bid, String wm, String cid, String t, String ps, String provTsmp)
    {
        super(t,cid);
 
        page_id = new String(pid);
        farm = new String(f);
        node = new String(n);
        blob_id = new String(bid);
        watermark = new String(wm);
        page_size = new String(ps);
        providerTimestamp = new String(provTsmp);
    }
    
    
    public ProviderReadData(String f, String n, String[] data)
    {
        super(data[4]);
        
        farm = f;
        node = n;
        
        // the monitored parameter:
        // page_id-----blob_id----watermark----page_size-------client------ provider timestamp
        page_id = data [0];
        blob_id = data[1];
        watermark = data[2];
        page_size = data[3];
        
        providerTimestamp = data[5];
        
    }
    
    public ProviderReadData(ProviderReadData pr)
    {
        super(pr.timestamp, pr.clientID);
        
        farm = pr.farm;
        node = pr.node;
        
        page_id = pr.page_id;
        blob_id = pr.blob_id;
        watermark = pr.watermark;
        page_size = pr.page_size;
        
        providerTimestamp = pr.providerTimestamp;
        
    }

	@Override
	public int getMonDataType()
	{
		return MonitorData.PROVIDER_READ;
	}

}
