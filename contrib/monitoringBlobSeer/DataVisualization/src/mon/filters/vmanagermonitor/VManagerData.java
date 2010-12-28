//
//  VManagerData.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//
package mon.filters.vmanagermonitor;

import mon.filters.util.MonitorData;

public class VManagerData extends MonitorData{

    public String blob_id;
    public String watermark;
    public String version;
    public String writeOffset;
    public String writeSize;

    
    // write message
    public VManagerData(String t, String cid, String ver, String bid, String wm, String off, String size)
    {
        super(t,cid);
        
        version = ver;
        blob_id = bid;
        watermark = wm;
        writeOffset = off;
	writeSize = size;
    }

    
    // the received data has the following structure:
    // 	timestamp+"#"+clientID+"#"+version+"#"+blobID+"#"+watermark+"#"+writeOffset+"#"+writeSize;
    public VManagerData(String[] data)
    {
        super(data[0], data[1]);
        
        version = data[2];
        blob_id = data[3];
        watermark = data[4];
        writeOffset = data[5];
	writeSize = data[6];
        
    }
    
}
