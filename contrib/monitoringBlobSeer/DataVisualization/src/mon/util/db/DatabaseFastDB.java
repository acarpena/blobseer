//
//  DatabaseFastDB.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//
package mon.util.db;

import lia.Monitor.Store.Fast.DB;

public class DatabaseFastDB implements DatabaseAccess {

    private DB db;
    
    public DatabaseFastDB()
    {
	initializeDB();
    }
    
    public void initializeDB()
    {
         db = new DB();
    }
    
    public void executeQuery(String sqlCommand)
    {
        db.query(sqlCommand);
    }
    
    public boolean next()
    {
        return db.moveNext();
    }
   
    
    public String getString(String colName)
    {
        return db.gets(colName);
    }
    
    public String getString(int colIndex)
    {
         return db.gets(colIndex);
    }
    
    public double getDouble(String colName)
    {
        return db.getd(colName);
    }
    
    public double getDouble(int colIndex)
    {
        return db.getd(colIndex);
    }
    
   
    public int getInt(String colName)
    {
        return db.geti(colName);
    }
    
    public int getInt(int colIndex)
    {
        return db.geti(colIndex);
    }
    
    
}
