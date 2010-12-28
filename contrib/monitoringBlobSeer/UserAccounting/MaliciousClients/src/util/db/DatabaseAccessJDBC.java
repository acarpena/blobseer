package util.db;

import java.io.*;
import java.sql.*;
import java.util.Properties;

import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;

import mon.util.db.DatabaseAccess;

public class DatabaseAccessJDBC implements DatabaseAccess 
{
	private Connection conn = null;
	private static  Logger logger = Logger.getLogger(DatabaseAccessJDBC.class.getName());
	private ResultSet rs;
	private  Properties properties;
	 
	public DatabaseAccessJDBC(String fileName)
	{
		properties = new Properties();
		try 
		{
		    properties.load(new FileInputStream(fileName));
		} 
		catch (IOException e) {}
		initializeDB();
	}
	
	public void executeQuery(String sqlQuery) 
	{
		try
		{
			Statement stat = conn.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
			stat.executeQuery(sqlQuery);
			rs = stat.getResultSet();
		} catch (SQLException e)
		{
			logger.error(e.toString());
		}
		
	}
	
	public boolean query(String sqlQuery) 
	{
		try
		{
			Statement stat = conn.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
			boolean st = stat.execute(sqlQuery, Statement.NO_GENERATED_KEYS);
			rs = stat.getResultSet();
			if (st)
				return true;
			
		} catch (SQLException e)
		{
			logger.error(e.toString());
		}
		return false;
		
	}

	public double getDouble(String ColumnName) 
	{
		double dTemp = 0;
		if (rs == null)
			return dTemp;
		try
		{
			dTemp = rs.getDouble(ColumnName);
			if (rs.wasNull())
				dTemp = 0;
		} catch (SQLException e)
		{
			logger.error(e.toString());
		}
		return dTemp;
	}

	public double getDouble(int ColumnIndex) 
	{
		double dTemp = 0;
		if (rs == null)
			return dTemp;
		try
		{
			dTemp = rs.getDouble(ColumnIndex);
			if (rs.wasNull())
				dTemp = 0;
		} catch (SQLException e)
		{
			logger.error(e.toString());
		}
		return dTemp;
	}

	public String getString(String ColumnName) 
	{
		String sTemp = "";
		if (rs == null)
			return sTemp;
		try
		{
			sTemp = rs.getString(ColumnName);
			if (rs.wasNull())
				sTemp = "";
		} catch (SQLException e)
		{
			sTemp = "";
			logger.error(e.toString());
		}
		return sTemp;
	}

	public String getString(int ColumnIndex) 
	{
		String sTemp = "";
		if (rs == null)
			return sTemp;
		try
		{
			sTemp = rs.getString(ColumnIndex);
			if (rs.wasNull())
				sTemp = "";
		} catch (SQLException e)
		{
			sTemp = "";
			logger.error(e.toString());
		}
		
		return sTemp;
	}

	public String getDbURL()
	{
		String driverString = properties.getProperty("lia.Monitor.jdbcDriverString");
		String dbURL = new String();
		
		if (driverString.indexOf("postgres") != -1)
			dbURL = "jdbc:postgresql://" + properties.getProperty("lia.Monitor.ServerName", "131.254.11.254") + ":" + properties.getProperty("lia.Monitor.DatabasePort", "5432") + "/" + properties.getProperty("lia.Monitor.DatabaseName", "mon_data");
		else
			if (driverString.indexOf("mysql") != -1)
				dbURL = "jdbc:mysql://" + properties.getProperty("lia.Monitor.ServerName", "131.254.11.254") + ":" + properties.getProperty("lia.Monitor.DatabasePort", "5432") + "/" + properties.getProperty("lia.Monitor.DatabaseName", "mon_data");
		
		return dbURL ;
	}
	
	public void initializeDB() 
	{
		logger.addAppender(new ConsoleAppender(new PatternLayout("%p [%t] %c (%F:%L) - %m%n"), ConsoleAppender.SYSTEM_OUT));
		try 
		{
			String dbURL = getDbURL();
			conn = DriverManager.getConnection(dbURL, properties.getProperty("lia.Monitor.UserName", "mon_user"),properties.getProperty("lia.Monitor.Pass", "mon_pass"));
		    // use connection
		} 
		catch (SQLException e) 
		{
		    // log error
			logger.error(e.toString()+ " "+getDbURL());
			e.printStackTrace();
		} 
		finally 
		{
		    if (conn == null) 
		    {
		        logger.error("Could not connect to database!");
		    }
		}
	}

	public boolean next() 
	{
		try
		{
			return rs.next();
		} catch (SQLException e)
		{
			logger.info(e.toString());
		}
		return false;
	}
	
}
