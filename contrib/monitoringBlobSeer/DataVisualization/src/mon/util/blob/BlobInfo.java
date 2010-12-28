//
//  BlobInfo.java
//  
//
//  Created by Alexandra Carpen-Amarie on 6/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//
package mon.util.blob;
import mon.util.db.*;

public class BlobInfo {

    private String blobID;
    private double pageSize;
    
    public BlobInfo(String b, double ps)
    {
        blobID = b;
        pageSize = ps;
    }
    
    public BlobInfo(double ps)
    {
        blobID = null;
        pageSize = ps;
    }
    
    
    public BlobInfo(DatabaseFastDB db, String b)
    {
        try
        {
            db.executeQuery("select page_size from blobs"
                            + " where blob_id='" + b +"'");
            db.next();
            
            blobID = b;
            pageSize = db.getDouble(1);
            
        }
        catch (Exception e)
        {
            blobID = null;
        }
    }
    
    
    public void setBlobID(String b)
    {
        blobID = b;
    }
    
    public String getBlobID()
    {
        return blobID;
    }
    
    public void setPageSize(double ps)
    {
        pageSize = ps;
    }
    
    public double getPageSize()
    {
        return pageSize;
    }
    
}
