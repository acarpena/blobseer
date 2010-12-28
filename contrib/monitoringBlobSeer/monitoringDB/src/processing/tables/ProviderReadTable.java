//
//  ProviderWritesTable.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 5/6/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//
package processing.tables;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Vector;

import util.TimeUtil;
import util.db.DatabaseAccess;
import data.MonitorData;
import data.ProviderReadData;

/**
 * class that handles operating with the tables storing data from providers 
 *
 */
public class ProviderReadTable extends TableInterface{

    public ProviderReadTable(DatabaseAccess dba)
    {
        super("prov_read", dba);
    }
    
    public void insert(MonitorData d)
    {
    	ProviderReadData data = (ProviderReadData)d;
    	
    	String sqlcmd="INSERT INTO "+ escapeName(table+"+"+data.clientID)
                +" (page_id,blob_id,timestamp,farm,node,watermark, page_size, provider_timestamp)"
                +"VALUES ("
                		+ format(data.page_id) + ","
                		+ format(data.blob_id)+","
                		+ TimeUtil.getTimeMillis(data.timestamp)+","
                		+ format(data.farm) +","
                		+ format(data.node) +"," 
                		+ format(data.watermark) +","
                		+ data.page_size+ "," 
                		+ data.providerTimestamp 
                		+ ")";
        
        db.executeQuery(sqlcmd);
    }
    
    public Vector<ProviderReadData> getData(String clientId, String timestamp_val)
    {
    	Vector <ProviderReadData> data_container = new Vector<ProviderReadData>();
    	
    	if (this.equals(null))
    		return null;
    	
    	String sqlcmd = "SELECT * FROM "+ escapeName(table + "+"+ clientId) 
    			+ " WHERE timestamp > "+ TimeUtil.getTimeMillis(timestamp_val);
    	ResultSet rs = db.query(sqlcmd);
    	try
		{
			while (rs.next())
			{
				ProviderReadData data = new ProviderReadData();
				data.clientID = clientId;
				data.page_id = rs.getString(1);
				data.blob_id = rs.getString(2);
				data.timestamp = rs.getString(3);
				data.farm = rs.getString(4);
				data.node = rs.getString(5);
				data.watermark = rs.getString(6);
				data.page_size = rs.getString(7);
				data.providerTimestamp = rs.getString(8);
				
				data_container.add(data);
			}
		} catch (SQLException e)
		{
			e.printStackTrace();
		}
		return data_container;
    }
    
    
    public void createTable(String clientId)
    {
    	// the monitored parameter:
        // page_id-----blob_id----watermark----page_size-------client------timestamp
        String sqlcmd="CREATE TABLE " + escapeName(table + "+" + clientId)
    				+ "("
    				+ "page_id VARCHAR(255),"
    				+ "blob_id VARCHAR(255),"
    				+ "timestamp BIGINT,"
    				+ "farm VARCHAR(255),"
    				+ "node VARCHAR(255),"
    				+ "watermark VARCHAR(255),"
    				+ "page_size BIGINT,"
    				+ "provider_timestamp BIGINT "
    				//+ ", PRIMARY KEY(blob_id, watermark)"
    				+ ")";
    	db.executeQuery(sqlcmd);    	
    }
    
}
