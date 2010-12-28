//
//  DatabaseAccess.java
//  
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//
package mon.util.db;

public interface DatabaseAccess {

    public void initializeDB();
    public void executeQuery(String sqlCommand);
    
    public boolean next();
    //public void previous();
    //public void first();
    //public void last();

    
    public String getString(String colName);
    public String getString(int colIndex);
    
    public double getDouble(String colName);
    public double getDouble(int colIndex);
    
}
