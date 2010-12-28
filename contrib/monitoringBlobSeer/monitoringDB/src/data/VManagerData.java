//
//  VManagerData.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//
package data;

import util.TimeUtil;

/**
 * contains the data collected from the BlobSeer version manager
 *
 */
@SuppressWarnings("serial")
public class VManagerData extends MonitorData{

    public String blob_id;
    public String watermark;
    public String version;
    public String writeOffset;
    public String writeSize;
    public String page_size;
    
    // write message
    public VManagerData(String t, String cid, String ver, String bid, String wm, String off, String size, String ps)
    {
        super(t,cid);
        
        version = new String(ver);
        blob_id = new String(bid);
        watermark = new String(wm);
        writeOffset = new String(off);
        writeSize = new String(size);
        page_size = new String(ps);
    }
    
    public VManagerData()
    {
    	clientID = new String();
    	timestamp = TimeUtil.now();
    	blob_id = new String();
        watermark = new String();
        version = new String();
        writeOffset = new String();
        writeSize = new String();
        page_size = new String();
    }
    
    // the received data has the following structure:
    // 	timestamp+"#"+clientID+"#"+version+"#"+blobID+"#"+watermark+"#"+writeOffset+"#"+writeSize+"#"+page_size;
    public VManagerData(String[] data)
    {
        super(data[0], data[1]);
        
        version = data[2];
        blob_id = data[3];
        watermark = data[4];
        writeOffset = data[5];
        writeSize = data[6];
        page_size = data[7];
        
    }

	@Override
	public int getMonDataType()
	{
		return MonitorData.VMANAGER_WRITE;
	}

}
