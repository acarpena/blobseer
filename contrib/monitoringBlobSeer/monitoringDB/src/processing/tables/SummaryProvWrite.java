//
//  SummaryProvWrite.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 5/21/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//
package processing.tables;

import util.TimeUtil;
import util.db.DatabaseAccess;
import data.MonitorData;
import data.SummaryProviderWriteData;

/**
 * class that handles operating with the tables storing data from providers 
 *
 */
public class SummaryProvWrite extends TableInterface{

    public SummaryProvWrite(DatabaseAccess dba)
    {
        super("prov_write_summary", dba);
    }
    
    public void insert(MonitorData d)
    {
    	SummaryProviderWriteData data = (SummaryProviderWriteData)d;
    	
    	String sqlcmd="INSERT INTO "+ escapeName(table+"+"+data.clientID)
                +" (blob_id,watermark,pagescount, page_size, writesizecount, firstdate, lastdate, timestamp)"
                +"VALUES ("
                		+ format(data.blob_id)+","
                		+ format(data.watermark) +","
                		+ data.pagescount+","
                		+ data.page_size+ ","
                		+ (data.pagescount * data.page_size)+ ","
                		+ data.firstdate + ","
                		+ data.lastdate + ","
                		+ data.timestamp
                		+ ")";
        
        db.executeQuery(sqlcmd);
    }
    

    
    public void createTable(String clientId)
    {
    	// the summary for provider writes:
        // blob_id----watermark----number of written pages----page_size-- the size of the written pages--first timestamp---last timestamp
    	String sqlcmd="CREATE TABLE " + escapeName(table + "+" + clientId)
    				+ "("
    				+ "blob_id VARCHAR(255),"
    				+ "watermark VARCHAR(255),"
    				+ "pagescount BIGINT,"
    				+ "page_size BIGINT,"
    				+ "writesizecount BIGINT,"
    				+ "firstdate BIGINT, "
    				+ "lastdate BIGINT, "
    				+ "timestamp BIGINT"
    				//+ ", PRIMARY KEY(blob_id, watermark)"
    				+ ")";
    	db.executeQuery(sqlcmd);    	
    }
    
}
