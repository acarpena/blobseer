//
//  TableInterface.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//
package processing.tables;

import data.MonitorData;
import util.db.DatabaseAccess;
import lia.web.utils.Formatare;

/**
 * generic class that references access to a specific table
 *
 */
public abstract class TableInterface {

    protected String table;
    protected DatabaseAccess db;
    
    public TableInterface(String t, DatabaseAccess dba)
    {
        table = t;
        db = dba;
    }
    
    protected String format(final String i)
    {
        if (i==null) 
            return "null";
        return "'"+Formatare.mySQLEscape(i)+"'";
    }
      
    public String escapeName(String name)
    {
    	return "\""+name+"\"";
    }
    
    public abstract void insert(MonitorData data);
    public abstract void createTable(String clientId);
    
}
