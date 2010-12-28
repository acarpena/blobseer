//
//  ProviderWritesTable.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//
package processing.tables;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Vector;

import util.TimeUtil;
import util.db.DatabaseAccess;
import data.MonitorData;
import data.ProviderData;

/**
 * class that handles operating with the tables storing data from providers 
 *
 */
public class ProviderWriteTable extends TableInterface{

    public ProviderWriteTable(DatabaseAccess dba)
    {
        super("provider_writes_client", dba);
    }
    
    public void insert(MonitorData d)
    {
    	ProviderData data = (ProviderData)d;
    	
    	String sqlcmd="INSERT INTO "+ escapeName(table+"_"+data.clientID)
                +" (blob_id,timestamp,farm,node,watermark,page_index,page_number, page_size)"
                +"VALUES ("+data.blob_id+","+TimeUtil.getTimeMillis(data.timestamp)+","+ format(data.farm) +","
                + format(data.node) +"," + format(data.watermark) +","
                + data.page_index+","+data.page_number+","+data.page_size+")";
        
        db.executeQuery(sqlcmd);
    }
    
    public Vector<ProviderData> getData(String clientId, String timestamp_val)
    {
    	Vector <ProviderData> data_container = new Vector<ProviderData>();
    	
    	if (this.equals(null))
    		return null;
    	
    	String sqlcmd = "SELECT * FROM "+ escapeName(table + "_"+ clientId) + " WHERE timestamp > '"+ TimeUtil.getTimeMillis(timestamp_val)+"'";
    	ResultSet rs = db.query(sqlcmd);
    	try
		{
			while (rs.next())
			{
				ProviderData data = new ProviderData();
				data.clientID = clientId;
				data.timestamp = rs.getString(2);
				data.blob_id = rs.getString(1);
				data.farm = rs.getString(3);
				data.node = rs.getString(4);
				data.page_index = rs.getString(6);
				data.page_size = rs.getString(8);
				data.page_number = rs.getInt(7);
				data.watermark = rs.getString(5);
				
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
    	String sqlcmd="CREATE TABLE " + escapeName(table + "_" + clientId)
    				+ "("
    				+ "blob_id BIGINT,"
    				+ "timestamp VARCHAR(255),"
    				+ "farm VARCHAR(255),"
    				+ "node VARCHAR(255),"
    				+ "watermark VARCHAR(255),"
    				+ "page_index BIGINT,"
    				+ "page_number BIGINT,"
    				+ "page_size BIGINT,"
    				+ "PRIMARY KEY(blob_id, watermark)"
    				+ ")";
    	db.executeQuery(sqlcmd);    	
    }
    
    private String escapeName(String name)
    {
    	return "\""+name+"\"";
    }
}
