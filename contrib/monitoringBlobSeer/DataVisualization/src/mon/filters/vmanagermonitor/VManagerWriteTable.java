//
//  VManagerWriteTable.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//
package mon.filters.vmanagermonitor;

import mon.filters.util.MonitorData;
import mon.filters.util.TableInterface;
import mon.filters.util.TimeUtil;
import mon.util.db.DatabaseAccess;

public class VManagerWriteTable extends TableInterface{

    public VManagerWriteTable(DatabaseAccess dba)
    {
        super("client_writes", dba);
    }
    
    public void insert(MonitorData d)
    {
    	VManagerData data = (VManagerData)d;
    	
    	//timestamp+"#"+clientID+"#"+version+"#"+blobID+"#"+watermark+"#"+writeOffset+"#"+writeSize;
        String sqlcmd="INSERT INTO " + table + "_"+data.blob_id
                +" (version, watermark, blob_offset, write_size, timestamp, client_id)"
                +"VALUES (" + format(data.version) +","
                + format(data.watermark) +"," + format(data.writeOffset) +","
                + format(data.writeSize)+"," + TimeUtil.getTimeMillis(data.timestamp) + ","
                + format(data.clientID) + ")";
        
        db.executeQuery(sqlcmd);
    }
    
    
    public void createTable(String blobID)
    {   	
    	String sqlcmd;
    	
    	sqlcmd="CREATE TABLE " + table + "_" + blobID
			+ "("
			+ "version INT,"
			+ "watermark VARCHAR(255),"
			+ "blob_offset BIGINT,"
			+ "write_size BIGINT,"
			+ "timestamp VARCHAR(255),"
			+ "client_id VARCHAR(255),"
			+ "PRIMARY KEY(watermark, blob_offset)"
			+ ")";
    	db.executeQuery(sqlcmd);	
    	
    }

}
