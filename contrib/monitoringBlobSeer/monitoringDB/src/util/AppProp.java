package util;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

public class AppProp
{
	private static Properties properties;
	
	public AppProp(String propFile)
	{
		properties = new Properties();
		try 
		{
			FileInputStream fis = new FileInputStream(propFile);
		    properties.load(fis);
		    fis.close();
		} 
		catch (IOException e) {}
	}
	
	public static String getProp(String propName)
	{
		return properties.getProperty(propName);
	}
}
