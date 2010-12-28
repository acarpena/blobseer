//
//  ProviderData.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//
package mon.filters.providermonitor;

import mon.filters.util.MonitorData;

public class ProviderData extends MonitorData{

    public String blob_id;
    public String page_index;
    public String watermark;
    public String farm;
    public String node;
    
    // write message
    public ProviderData(String f, String n, String bid, String index, String wm, String cid, String t)
    {
        super(t,cid);
 
        farm = f;
        node = n;
        blob_id = bid;
        page_index = index;
        watermark = wm;
    }

    // read message - no watermark information
    public ProviderData(String f, String n, String bid, String index, String cid, String t)
    {
        super(t, cid);
        
        farm = f;
        node = n;
        blob_id = bid;
        page_index = index;
        watermark = "";
    }
    
    public ProviderData(String f, String n, String[] data)
    {
        super(data[data.length-1], data[data.length-2]);
        
        farm = f;
        node = n;
        
        blob_id = data[0];
        page_index = data [1];
        
        if (data.length == 5)
        {
            // write message - it also has the watermark
             watermark = data[2];
        }
    }
    
}
