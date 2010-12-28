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
import data.ProviderWriteData;
import data.SummaryProviderWriteData;

/**
 * class that handles operating with the tables storing data from providers 
 *
 */
public class ProviderWriteTable extends TableInterface{	
	
    public ProviderWriteTable(DatabaseAccess dba)
    {
        super("prov_write", dba);
    }
    
    public void insert(MonitorData d)
    {
    	ProviderWriteData data = (ProviderWriteData)d;
    	
    	String sqlcmd="INSERT INTO "+ escapeName(table+"+"+data.clientID)
                +" (blob_id,timestamp,farm,node,watermark,page_index, page_size, provider_timestamp)"
                +"VALUES ("
                		+ format(data.blob_id)+","
                		+ TimeUtil.getTimeMillis(data.timestamp) +","
                		+ format(data.farm) +","
                		+ format(data.node) +"," 
                		+ format(data.watermark) +","
                		+ data.page_index+","
                		+ data.page_size+ "," 
                		+ data.providerTimestamp
                		+ ")";
        
        db.executeQuery(sqlcmd);
    }
    
    public Vector<ProviderWriteData> getData(String clientId, String timestamp_val)
    {
    	Vector <ProviderWriteData> data_container = new Vector<ProviderWriteData>();
    	
    	if (this.equals(null))
    		return null;
    	
    	String sqlcmd = "SELECT * FROM "+ escapeName(table + "+"+ clientId) 
    					+ " WHERE timestamp > "+ TimeUtil.getTimeMillis(timestamp_val);
    	ResultSet rs = db.query(sqlcmd);
    	try
		{
			while (rs.next())
			{
				ProviderWriteData data = new ProviderWriteData();
				data.clientID = clientId;
				data.blob_id = rs.getString(1);
				data.timestamp = rs.getString(2);
				data.farm = rs.getString(3);
				data.node = rs.getString(4);
				data.watermark = rs.getString(5);
				data.page_index = rs.getString(6);
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
        // farm----node---timestamp---blob_id----index---watermark----page_size-------client------provider timestamp
    	String sqlcmd="CREATE TABLE " + escapeName(table + "+" + clientId)
    				+ "("
    				+ "blob_id VARCHAR(255),"
    				+ "timestamp BIGINT,"
    				+ "farm VARCHAR(255),"
    				+ "node VARCHAR(255),"
    				+ "watermark VARCHAR(255),"
    				+ "page_index BIGINT,"
    				+ "page_size BIGINT,"
    				+ "provider_timestamp BIGINT "
    				//+ ", PRIMARY KEY(blob_id, watermark)"
    				+ ")";
    	db.executeQuery(sqlcmd);    	
    }
    
    
	public Vector<SummaryProviderWriteData> getUpdates(String clientID, String timestamp) {
		
		if (timestamp == null || timestamp.equals(""))
		{
			timestamp = TimeUtil.getInitTimestamp();
		}
		
		// TODO -> must be modified to avoid counting the same page 
		// (different records related to the same page) in 2 different updates
		String sql_requestUpdates = 
			"SELECT blob_id," +
			" watermark, " +
			" min(page_size) as page_size, " +
			" min(timestamp) as firstdate, " +
			" max(timestamp) as lastdate," +
			" count(page_index) as pagescount " +
			" FROM " +
				"( SELECT blob_id, watermark, page_index, min(page_size) as page_size, " +
				"max(timestamp) as timestamp, min(timestamp) as checktimestamp" +
				" FROM " + escapeName(table + "+" + clientID) +
				" WHERE timestamp > " + TimeUtil.getTimeMillis(timestamp) + " - 1000 " +
				" GROUP BY blob_id, watermark, page_index" +
				")" +
				" AS prov_write" +
			" WHERE checktimestamp > " + TimeUtil.getTimeMillis(timestamp) +
			" GROUP BY  blob_id, watermark";

		ResultSet rs = db.query(sql_requestUpdates);
		Vector <SummaryProviderWriteData> data_container = new Vector<SummaryProviderWriteData>();
    	
		try
		{
			while (rs.next())
			{
				SummaryProviderWriteData data = new SummaryProviderWriteData();
				data.clientID = clientID;
				data.blob_id = rs.getString(1);
				data.watermark = rs.getString(2);
				data.page_size = rs.getLong(3);
				data.firstdate = rs.getString(4);
				data.lastdate = rs.getString(5);
				data.pagescount = rs.getLong(6);
				
				data_container.add(data);
			}
		} catch (SQLException e)
		{
			System.out.println("Exception in select for updates: "+ sql_requestUpdates);
			e.printStackTrace();
		}
		
		System.out.println("Finished updating for clientId= "+ clientID +" timestamp="+ timestamp + " result size="+ data_container.size());
		return data_container;
	}
}
