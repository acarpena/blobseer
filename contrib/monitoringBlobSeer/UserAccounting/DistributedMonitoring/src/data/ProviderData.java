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
@SuppressWarnings("serial")
public class ProviderData extends MonitorData
{
    public String blob_id;
    public String page_index;
    public String watermark;
    public String farm;
    public String node;
    public String page_size;
    public int page_number;
    
    // write message
    public ProviderData()
    {
    	clientID = new String();
    	timestamp = new String();
    	blob_id = new String();
    	page_index = new String();
    	watermark = new String();
    	farm = new String();
    	node = new String();
    	page_size = new String();
    	page_number = 1;
    }
    
    public ProviderData(String f, String n, String bid, String index, String wm, String cid, String t, String ps)
    {
        super(t,cid);
 
        farm = new String(f);
        node = new String(n);
        blob_id = new String(bid);
        page_index = new String(index);
        watermark = new String(wm);
        page_size = new String(ps);
        page_number = 1;
    }

    // read message - no watermark information
    public ProviderData(String f, String n, String bid, String index, String cid, String t, String ps)
    {
        super(t, cid);
        
        farm = f;
        node = n;
        blob_id = bid;
        page_index = index;
        watermark = "";
        page_size = ps;
        page_number = 1;
    }
    
    public ProviderData(String f, String n, String[] data)
    {
        super(data[data.length-2], data[data.length-3]);
        
        farm = f;
        node = n;
        
        blob_id = data[0];
        page_index = data [1];
        
        if (data.length == 6)
        {
            // write message - it also has the watermark
             watermark = data[2];
        }
        page_size = data[data.length - 1];
        page_number = 1;
    }

	@Override
	public String getMonDataType()
	{
		return PROVIDER_STRING;
	}

	@Override
	public MonitorData clone()
	{
		ProviderData data = new ProviderData( farm , node,blob_id, page_index, watermark, clientID, timestamp,page_size);
		data.page_number = page_number;
		data.page_index = data.page_index;
		return data;
	}

}
