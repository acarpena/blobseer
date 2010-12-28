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
import data.MetadataProvNodesData;

/**
 * class that handles operating with the tables storing data from providers 
 *
 */
public class MetadataProvNodesTable extends TableInterface{

    public MetadataProvNodesTable(DatabaseAccess dba)
    {
        super("mprov_node_requests", dba);
    }
    
    public void insert(MonitorData d)
    {
    	MetadataProvNodesData data = (MetadataProvNodesData)d;
    	
    	String sqlcmd="INSERT INTO "+ escapeName(table+"+"+data.clientID)
                +" (nodes,mprov_id,req_timestamp,timestamp)"
                +"VALUES ("
                		+ data.number_of_nodes + ","
                		+ format(data.node)+","
                		+ data.reqTimestamp+","
                		+ TimeUtil.getTimeMillis(data.timestamp)
                		+ ")";
        
        db.executeQuery(sqlcmd);
    }
    
    public Vector<MetadataProvNodesData> getData(String clientId, String timestamp_val)
    {
    	Vector <MetadataProvNodesData> data_container = new Vector<MetadataProvNodesData>();
    	
    	if (this.equals(null))
    		return null;
    	
    	String sqlcmd = "SELECT * FROM "+ escapeName(table + "+"+ clientId) + 
    				" WHERE timestamp > "+ TimeUtil.getTimeMillis(timestamp_val);
    	ResultSet rs = db.query(sqlcmd);
    	try
		{
			while (rs.next())
			{
				MetadataProvNodesData data = new MetadataProvNodesData();
				data.clientID = clientId;
				data.number_of_nodes = rs.getString(1);
				data.node = rs.getString(2);
				data.reqTimestamp = rs.getString(3);
				data.timestamp = rs.getString(4);
				
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
    	// number of requested nodes--prov id------request timestamp (to filter duplicates sent to different ML farms)---client id
        String sqlcmd="CREATE TABLE " + escapeName(table + "+" + clientId)
    				+ "("
    				+ "nodes BIGINT,"
    				+ "mprov_id VARCHAR(255),"
    				+ "req_timestamp BIGINT, "
    				+ "timestamp BIGINT "
    				//+ ", PRIMARY KEY(blob_id, watermark)"
    				+ ")";
    	db.executeQuery(sqlcmd);    	
    }
    
}
