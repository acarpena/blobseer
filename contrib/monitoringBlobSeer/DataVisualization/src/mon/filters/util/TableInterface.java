//
//  TableInterface.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//
package mon.filters.util;

import lia.web.utils.Formatare;
import mon.util.db.DatabaseAccess;

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
    
    
    public abstract void insert(MonitorData data);
    public abstract void createTable(String blobID);
     
}
