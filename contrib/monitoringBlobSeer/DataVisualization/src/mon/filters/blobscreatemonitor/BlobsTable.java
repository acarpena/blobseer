//
//  BlobsTable.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//
package mon.filters.blobscreatemonitor;

import mon.filters.util.MonitorData;
import mon.filters.util.TableInterface;
import mon.filters.util.TimeUtil;
import mon.util.db.DatabaseAccess;

public class BlobsTable extends TableInterface{

    public BlobsTable(DatabaseAccess dba)
    {
        super("blobs", dba);
    }
    
    public void insert(MonitorData d)
    {
    	BlobsData data = (BlobsData)d;
    	
    	//timestamp+"#"+clientID+"#"+blobID+"#"+pageSize+"#"+repFactor;
    	String sqlcmd="INSERT INTO " + table
                +" (timestamp, client_id, blob_id, page_size, replica_count)"
                +"VALUES (" + TimeUtil.getTimeMillis(data.timestamp) +","
                + format(data.clientID) +"," + format(data.blob_id) +","
                + format(data.pageSize)+"," + format(data.replicas) + ")";
        
        db.executeQuery(sqlcmd);
    }
    
    
    public void createTable(String blobID)
    {   	
    	String sqlcmd;
    	
    	sqlcmd="CREATE TABLE " + table
			+ "("
			+ "timestamp VARCHAR(255),"
			+ "client_id VARCHAR(255),"
			+ "blob_id VARCHAR(255),"
			+ "page_size BIGINT,"
			+ "replica_count INT,"
			+ "PRIMARY KEY(blob_id)"
			+ ")";
    	db.executeQuery(sqlcmd);	
    	
    }

}
