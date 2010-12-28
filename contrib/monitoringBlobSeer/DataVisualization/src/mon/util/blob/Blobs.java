//
//  Blobs.java
//  
//
//  Created by Alexandra Carpen-Amarie on 6/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//
package mon.util.blob;

import java.util.HashMap;
import java.util.ArrayList;

import mon.util.db.DatabaseAccess;


/**
 * @author acarpena
 *
 * Class that retrieves all the blobs (and their respective page sizes) from the database
 */
public class Blobs {
    
    // store the list of the blobs
    private HashMap<String, BlobInfo> blobs;
    
    public Blobs(DatabaseAccess db)
    {
        blobs = new HashMap<String, BlobInfo>();
        
        // retrieve the blobs and their attributes from the BLOBS table in the database
        try
        {
            db.executeQuery("select blob_id, page_size from blobs");
            
            while(db.next())
            {
                blobs.put(db.getString(1), new BlobInfo(db.getString(1), db.getDouble(2)));
            }
            
        }
        catch (Exception e)
        {
           // exception               
        }
    }
    
    
    public ArrayList<String> getBlobIDs()
    {
        return new ArrayList<String>(blobs.keySet());
    }
    
    public ArrayList<BlobInfo> getBlobInfos()
    {
        return new ArrayList<BlobInfo>(blobs.values());
    }
    
    public double getBlobPageSize(String id)
    {
        return blobs.get(id).getPageSize();
    }
    
    public int getBlobsNumber()
    {
    	return blobs.size();
    }
    
}
