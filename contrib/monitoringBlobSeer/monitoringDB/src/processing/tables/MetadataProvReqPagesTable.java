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
import data.MetadataProvReqPagesData;

/**
 * class that handles operating with the tables storing data from providers 
 *
 */
public class MetadataProvReqPagesTable extends TableInterface{

    public MetadataProvReqPagesTable(DatabaseAccess dba)
    {
        super("mprov_page_id_requests", dba);
    }
    
    public void insert(MonitorData d)
    {
    	MetadataProvReqPagesData data = (MetadataProvReqPagesData)d;
    	
    	String sqlcmd="INSERT INTO "+ escapeName(table+"+"+data.clientID)
                +" (page_id,req_timestamp,timestamp)"
                +"VALUES ("
                		+ format(data.page_id) + ","
                		+ format(data.reqTimestamp)+","
                		+ TimeUtil.getTimeMillis(data.timestamp)
                		+ ")";
        
        db.executeQuery(sqlcmd);
    }
    
    public Vector<MetadataProvReqPagesData> getData(String clientId, String timestamp_val)
    {
    	Vector <MetadataProvReqPagesData> data_container = new Vector<MetadataProvReqPagesData>();
    	
    	if (this.equals(null))
    		return null;
    	
    	String sqlcmd = "SELECT * FROM "+ escapeName(table + "+"+ clientId) 
    			+ " WHERE timestamp > "+ TimeUtil.getTimeMillis(timestamp_val);
    	ResultSet rs = db.query(sqlcmd);
    	try
		{
			while (rs.next())
			{
				MetadataProvReqPagesData data = new MetadataProvReqPagesData();
				data.clientID = clientId;
				data.page_id = rs.getString(1);
				data.reqTimestamp = rs.getString(2);
				data.timestamp = rs.getString(3);
				
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
    	// page id----request timestamp (to filter duplicates sent to different ML farms)---client id
    	String sqlcmd="CREATE TABLE " + escapeName(table + "+" + clientId)
    				+ "("
    				+ "page_id VARCHAR(255),"
    				+ "req_timestamp VARCHAR(255),"
    				+ "timestamp BIGINT"
    				//+ ", PRIMARY KEY(blob_id, watermark)"
    				+ ")";
    	db.executeQuery(sqlcmd);    	
    }
    
}

