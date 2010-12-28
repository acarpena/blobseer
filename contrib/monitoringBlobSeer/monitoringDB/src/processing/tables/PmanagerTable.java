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
import data.ProvManagerData;

/**
 * class that handles operating with the tables storing data from providers 
 *
 */
public class PmanagerTable extends TableInterface{

    public PmanagerTable(DatabaseAccess dba)
    {
        super("pman_requested_prov", dba);
    }
    
    public void insert(MonitorData d)
    {
    	ProvManagerData data = (ProvManagerData)d;
    	
    	String sqlcmd="INSERT INTO "+ escapeName(table+"+"+data.clientID)
                +" (provider_id,request_id,timestamp)"
                +"VALUES ("
                		+ format(data.prov_id) + ","
                		+ format(data.request_id)+","
                		+ TimeUtil.getTimeMillis(data.timestamp)
                		+ ")";
        
        db.executeQuery(sqlcmd);
    }
    
    public Vector<ProvManagerData> getData(String clientId, String timestamp_val)
    {
    	Vector <ProvManagerData> data_container = new Vector<ProvManagerData>();
    	
    	if (this.equals(null))
    		return null;
    	
    	String sqlcmd = "SELECT * FROM "+ escapeName(table + "+"+ clientId) 
    				+ " WHERE timestamp > "+ TimeUtil.getTimeMillis(timestamp_val);
    	ResultSet rs = db.query(sqlcmd);
    	try
		{
			while (rs.next())
			{
				ProvManagerData data = new ProvManagerData();
				data.clientID = clientId;
				data.prov_id = rs.getString(1);
				data.request_id = rs.getString(2);
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
        // prov_id (hostname), requestId (a random timestamp), clientId
        String sqlcmd="CREATE TABLE " + escapeName(table + "+" + clientId)
    				+ "("
    				+ "provider_id VARCHAR(255),"
    				+ "request_id VARCHAR(255),"
    				+ "timestamp BIGINT"
    				//+ ", PRIMARY KEY(blob_id, watermark)"
    				+ ")";
    	db.executeQuery(sqlcmd);    	
    }
    
}
