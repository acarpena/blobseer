package mon.filters.util;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class TimeUtil {
    
	public static String now()
	{
		String DATE_FORMAT_NOW = "yyyy-MMM-dd HH:mm:ss.SSSSSS";
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
	
	
	// transform the formatted time into a timestamp (milliseconds from the epoch)
	public static long getTimeMillis (String time)
	{
		String DATE_FORMAT_NOW = "yyyy-MMM-dd HH:mm:ss.SSSSSS";
	    SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT_NOW);
		Date d = new Date();
		try {
			d = sdf.parse(time);
		} catch (ParseException e) {
			e.printStackTrace();
		}
	    
	    return d.getTime();
	}
	
}
