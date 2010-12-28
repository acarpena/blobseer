package vmonitor;

import java.text.SimpleDateFormat;
import java.util.Calendar;

public class TimeUtil{

	public static String now()
	{
		String DATE_FORMAT_NOW = "yyyy-MM-dd HH:mm:ss";
	    Calendar cal = Calendar.getInstance();
	    SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT_NOW);
	    return sdf.format(cal.getTime());
	}
	
	public static long nowMillis(){
		return System.currentTimeMillis();
	}
	
	public static long nowMicros(){
		return System.currentTimeMillis()*1000;
	}
	
}
