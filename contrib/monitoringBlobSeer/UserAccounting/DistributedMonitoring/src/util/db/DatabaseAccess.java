//
//  DatabaseAccess.java
//  
//
//  Created by Alexandra Carpen-Amarie on 6/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//
package util.db;

import java.sql.ResultSet;

public interface DatabaseAccess {

    public void initializeDB();
    public void executeQuery(String sqlCommand);
    public ResultSet query(String sqlQuery) ;
    
    public boolean next();
    //public void previous();
    //public void first();
    //public void last();

    
    public String getString(String colName);
    public String getString(int colIndex);
    
    public double getDouble(String colName);
    public double getDouble(int colIndex);
    
}
