package vmonitor;

import java.util.*;
import java.io.*;
import java.util.regex.*;
import apmon.*;
import java.util.logging.Logger;



public class VManagerMonitor
{
	private int sleeptime = 1000;
    private ApMon apm = null;
	private	String logFile = null;
	private	RandomAccessFile raf = null;
	private long fileOffset = 0; 
	private static Logger logger = Logger.getLogger("vmanagermonitor");
	
	public VManagerMonitor(String logFile, String apmFile)
	{
		this.logFile = logFile;
		try{
			apm = new ApMon(apmFile);
		}catch (Exception e){
			logger.severe("Error initializing ApMon: " + e);
	        System.exit(-1);
		}	
	}

    public VManagerMonitor(String logFile, String apmFile, int sleep)
	{
		this.logFile = logFile;
        this.sleeptime = sleep;
		try{
			apm = new ApMon(apmFile);
		}catch (Exception e){
			logger.severe("Error initializing ApMon: " + e);
	        System.exit(-1);
		}	
	}
    
    
    //    [INFO 2009-Jul-21 15:23:20.073836] [/Users/acarpena/Work/blobseer/tags/release-0.3/vmanager/vmanagement.cpp:120:get_ticket] RPC success: allocated a new version (9) for request (1, 1753513934, 267386880, 268435456) {CAV}


	private void processLine(String line)
	{	//log line pattern:
		//[INFO date][method within source file][client id] information (..,..,..) {operation}
		
		Pattern p = Pattern.compile("\\[INFO (.+)] \\[.*] \\[(.*)](\\D)*([0-9]+)?.*(\\(([0-9]+), ([0-9]+), ([0-9]+)(, )?([0-9]+)?\\)) \\{(.+)}");
	    Matcher m = p.matcher(line);

	    // if we find a match, get the group 
	    if (m.find())
	    {  
	    	if((m.group(m.groupCount())).contains("CCB")){
	    		VManagerCCB DataCCB=new VManagerCCB(m.group(1), m.group(2), Long.parseLong(m.group(6)), Long.parseLong(m.group(7)), Integer.parseInt(m.group(8)));

	    		logger.info("Sending createBlob information read at " + DataCCB.timestamp);
	    		 try {
	                 apm.sendParameter("VManager", null, "blobCreate", ApMon.XDR_STRING, DataCCB.toString());
	             } catch(Exception e) {
	                 logger.warning("Send createBlob operation failed: " + e);
	             } 
	    	}
	    	if((m.group(m.groupCount())).contains("CAV")){
	    		VManagerCAV DataCAV=new VManagerCAV(m.group(1), m.group(2), Long.parseLong(m.group(4)), Long.parseLong(m.group(6)), m.group(7), Long.parseLong(m.group(8)), Long.parseLong(m.group(10)));

                logger.info("Sending writeBlob information read at " + DataCAV.timestamp);
	    		 try {
	                 apm.sendParameter("VManager", null, "blobWrite", ApMon.XDR_STRING, DataCAV.toString());
	             } catch(Exception e) {
	                 logger.warning("Send writeBlob operation failed: " + e);
	             } 
	    	}
	    } 
	}
	
	private void processLog()
	{
		int sleeping=0;
		try 
		 {
			 raf = new RandomAccessFile (logFile, "r");
	     } 
		 catch (FileNotFoundException e) 
		 {
			 logger.severe("Log file "+logFile+" not found: " + e);
	         return;
	     }
		 try
		 {
			 while(true)
		     {
		    	 if(sleeping==1){
		    		 try 
		    		 {
		    			 raf = new RandomAccessFile (logFile, "r");
		    	     } 
		    		 catch (FileNotFoundException e) 
		    		 {
		    			 logger.severe("Log file "+logFile+" not found: " + e);
		    	         return;
		    	     }
		    		 raf.seek(fileOffset);
		    		 String line="";
		    		 line=raf.readLine();
		    		 if(line==null){
		    			 logger.info("VManagerMonitor sleeping... reached end of log file");
		    			 //preparations before sleep
		    			 sleeping=1;
		    			 fileOffset=raf.getFilePointer();
		    			 raf.close();
		    			 Thread.sleep(sleeptime);
		    		 }
		    		 else{
		    			 sleeping=0;
		    			 if(line.endsWith("}"))
		    				 processLine(line);
		    		 }
		    	 }else{
		    		 String line="";
		    		 line=raf.readLine();
		    		 if(line==null){
		    			 logger.info("VManagerMonitor sleeping... reached end of log file");
		    			 //preparations before sleep
		    			 sleeping=1;
		    			 fileOffset=raf.getFilePointer();
		    			 raf.close();
		    			 Thread.sleep(sleeptime);
		    		 }
		    		 else if(line.endsWith("}"))
		    			 processLine(line);
		    	 }
		     }
			 
		 }
		 catch(Exception e)
		 {
			 e.printStackTrace();
		 }
		 apm.stopIt();  
	}
	
	public static void main(String[] args)
	{
        VManagerMonitor mon = null;
        
        if ((args.length < 2) || (args.length > 3))
        {
            System.err.println("Usage: java vmonitor/VManagerMonitor log_path apMon_conf_file [sleepTime_ms]");
            System.exit(1);
        }
        
		String log = args[0];
        String apmonConf = args[1];
		
        if (args.length == 2)
        {
            mon=new VManagerMonitor(log, apmonConf);
        }
        else
        {
            mon=new VManagerMonitor(log, apmonConf, Integer.valueOf(args[2]));
        }
        
        mon.processLog(); 
	}
}
