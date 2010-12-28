//
//  ProviderWritesTable.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//
package mon.filters.providermonitor;

import mon.filters.util.MonitorData;
import mon.filters.util.TableInterface;
import mon.filters.util.TimeUtil;
import mon.util.db.DatabaseAccess;

public class ProviderWriteTable extends TableInterface{

    public ProviderWriteTable(DatabaseAccess dba)
    {
        super("write", dba);
    }
    
    public void insert(MonitorData d)
    {
    	ProviderData data = (ProviderData)d;
    	
    	String sqlcmd="INSERT INTO "+table+"_"+data.blob_id
                +" (timestamp,farm,node,watermark,page_index,client_id)"
                +"VALUES ("+TimeUtil.getTimeMillis(data.timestamp)+","+ format(data.farm) +","
                + format(data.node) +"," + format(data.watermark) +","
                + data.page_index+"," + format(data.clientID) +")";
        
        db.executeQuery(sqlcmd);
    }
    
    
    public void createTable(String blobID)
    {
    	String sqlcmd="CREATE TABLE " + table + "_" + blobID
    				+ "("
    				+ "timestamp VARCHAR(255),"
    				+ "farm VARCHAR(255),"
    				+ "node VARCHAR(255),"
    				+ "watermark VARCHAR(255),"
    				+ "page_index BIGINT,"
    				+ "client_id VARCHAR(255),"
    				+ "PRIMARY KEY(watermark, page_index)"
    				+ ")";
    	db.executeQuery(sqlcmd);    	
    }

}
