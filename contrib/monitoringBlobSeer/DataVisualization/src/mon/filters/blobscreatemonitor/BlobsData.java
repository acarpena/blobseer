//
//  BlobsData.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//
package mon.filters.blobscreatemonitor;

import mon.filters.util.MonitorData;

public class BlobsData extends MonitorData{

    public String blob_id;
    public String pageSize;
    public String replicas;
    
    public BlobsData(String cid, String t, String bid, String size, String r)
    {
        super(t,cid);
 
        blob_id = bid;
        pageSize = size;
        replicas = r;
    }
  
    public BlobsData(String[] data)
    {
        super(data[0], data[1]);
        
        blob_id = data[2];
        pageSize = data [3];
        replicas = data[4];
        
    }
    
}