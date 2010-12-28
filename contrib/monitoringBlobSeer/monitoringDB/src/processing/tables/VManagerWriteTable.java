//
//  VManagerWriteTable.java
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
import data.VManagerData;

/**
 * class that handles operations with the table storing data from the version manager monitor
 */
public class VManagerWriteTable extends TableInterface{

    public VManagerWriteTable(DatabaseAccess dba)
    {
        super("vman_write", dba);
    }
    
    public void insert(MonitorData d)
    {
    	VManagerData data = (VManagerData)d;
    	
    	//timestamp+"#"+clientID+"#"+version+"#"+blobID+"#"+watermark+"#"+writeOffset+"#"+writeSize;
        String sqlcmd="INSERT INTO " + escapeName(table+"+"+data.clientID) 
                +" (client_id, blob_id, version, watermark, blob_offset, write_size, page_size, timestamp)"
                +"VALUES (" 
                + format(data.blob_id)+"," 
                + format(data.version) +","
                + format(data.watermark) +"," 
                + format(data.writeOffset) +","
                + format(data.writeSize)+"," 
                + format(data.page_size)+","
                + TimeUtil.getTimeMillis(data.timestamp) 
                + ")";
        
        db.executeQuery(sqlcmd);
    }
    
    
    public void createTable(String clientId)
    {   	
    	String sqlcmd;
    	
    	sqlcmd="CREATE TABLE " + escapeName(table+"+"+clientId) 
			+ "("
			+ "blob_id BIGINT,"
			+ "version VARCHAR(255),"
			+ "watermark VARCHAR(255),"
			+ "blob_offset BIGINT,"
			+ "write_size BIGINT,"
			+ "page_size BIGINT,"
			+ "timestamp VARCHAR(255),"
			+ "PRIMARY KEY(watermark, blob_offset)"
			+ ")";
    	db.executeQuery(sqlcmd);	
    }
    
    public Vector<VManagerData> getData(String client_id, String timestamp_val)
    {
    	Vector<VManagerData> data_container= new Vector<VManagerData>();
    	if (this.equals(null))
    		return null;
    	
    	String sqlcmd = "SELECT * FROM "+ escapeName(table+"+"+client_id)
    				+ " WHERE timestamp > "+ TimeUtil.getTimeMillis(timestamp_val);
    	ResultSet rs = db.query(sqlcmd);
    	try
		{
			while (rs.next())
			{
				VManagerData data = new VManagerData();
				data.clientID = client_id;
				data.timestamp = rs.getString(8);
				data.blob_id = rs.getString(2);
				data.version = rs.getString(3);
				data.writeOffset = rs.getString(5);
				data.writeSize = rs.getString(6);
				data.page_size = rs.getString(7);
				data.watermark = rs.getString(4);
				
				data_container.add(data);
			}
		} catch (SQLException e)
		{
			e.printStackTrace();
		}
		
    	return data_container;
    }

}
