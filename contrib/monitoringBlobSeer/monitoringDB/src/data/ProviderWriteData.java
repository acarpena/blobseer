//
//  ProviderData.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//
package data;

/**
 * encapsulates the the monitoring information sent by BlobSeer providers
 * 
 */
public class ProviderWriteData extends MonitorData
{
    /**
	 * 
	 */
	private static final long serialVersionUID = 534270918834772712L;
	public String blob_id;
    public String page_index;
    public String watermark;
    public String farm;
    public String node;
    public String page_size;
    public String providerTimestamp;
    
    // write message
    public ProviderWriteData()
    {
    	clientID = new String();
    	timestamp = new String();
    	blob_id = new String();
    	page_index = new String();
    	watermark = new String();
    	farm = new String();
    	node = new String();
    	page_size = new String();
    	providerTimestamp = new String();
    }
    
    public ProviderWriteData(String f, String n, String bid, String index, String wm, String cid, String t, String ps, String provTsmp)
    {
        super(t,cid);
 
        farm = new String(f);
        node = new String(n);
        blob_id = new String(bid);
        page_index = new String(index);
        watermark = new String(wm);
        page_size = new String(ps);
        providerTimestamp = new String(provTsmp);
    }
    
    public ProviderWriteData(String f, String n, String[] data)
    {
        super(data[4]);
        
        farm = f;
        node = n;
        
        // the monitored parameter:
        // blob_id----index---watermark----page_size-------client------provider timestamp
        blob_id = data[0];
        page_index = data [1];
        watermark = data[2];
        page_size = data[3];
        
        providerTimestamp = data[5];
        
    }
    
    public ProviderWriteData(ProviderWriteData pw)
    {
        super(pw.timestamp, pw.clientID);
        
        farm = pw.farm;
        node = pw.node;
        
        blob_id = pw.blob_id;
        page_index = pw.page_index;
        watermark = pw.watermark;
        page_size = pw.page_size;
        
        providerTimestamp = pw.providerTimestamp;
        
    }

	@Override
	public int getMonDataType()
	{
		return MonitorData.PROVIDER_WRITE;
	}

}
