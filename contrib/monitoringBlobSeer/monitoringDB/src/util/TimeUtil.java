package util;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

/**
 * used in generating and working with timestamps 
 * 
 * @author Michi
 */
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
	

    // get the lowest possible timestamp
    public static String getInitTimestamp ()
    {
        return "1970-JAN-01 00:00:00.000000";
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
	
	/**
	 * returns the amount of time between the two dates, in milliseconds
	 * @param time1
	 * @param time2 
	 * @return
	 */
	public static long getTimeMillis (String time1, String time2) 
	{
		String DATE_FORMAT_NOW = "yyyy-MMM-dd HH:mm:ss.SSSSSS";
	    SimpleDateFormat sdf1 = new SimpleDateFormat(DATE_FORMAT_NOW);
	    SimpleDateFormat sdf2 = new SimpleDateFormat(DATE_FORMAT_NOW);
		Date d1 = new Date();
		Date d2 = new Date();
		try {
			d1 = sdf1.parse(time1);
			d2 = sdf2.parse(time2);
		} catch (ParseException e) {
			e.printStackTrace();
		}
	    
		long res = Math.abs(d1.getTime() - d2.getTime());
	    return res;
	}
	
}
