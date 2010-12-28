package feedback.userAccounting;

import java.io.IOException;
import java.net.ServerSocket;
import java.util.Vector;

import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;

import util.db.DatabaseAccessJDBC;

import mon.util.db.DatabaseAccess;

import feedback.userAccounting.blackList.BasicBlackList;
import feedback.userAccounting.listUpdateTimer.TimerUpdate;
import feedback.userAccounting.maliciousClients.MaliciousClients;

public class MaliciousClientsFeedback 
{

	 private static Logger logger = Logger.getLogger(BasicBlackList.class.getName());
	/**
	 * @param args
	 */
	public static void main(String[] args) 
	{
		ServerSocket serverSocket = null;
		BasicBlackList blackList;
		TimerUpdate timer;
	    
		
		if ((args.length != 2) && (args.length != 3))	{
			System.out.println("Usage: java MaliciousClientsFeedback port properties_file [updateDelay (sec)]");
			System.exit(1);
		}
		
		
		logger.addAppender(new ConsoleAppender(new PatternLayout("%p [%t] %c (%F:%L) - %m%n"), ConsoleAppender.SYSTEM_OUT));
		BasicBlackList.logger = logger;
		
        try 
        {
            serverSocket = new ServerSocket(Integer.valueOf(args[0]));
        } catch (IOException e) 
        {
            System.err.println("Could not listen on port "+args[0]);
            System.exit(-1);
        }
        
        Vector<DatabaseAccess> databaseList = new Vector<DatabaseAccess>();
        for (int i = 0; i < 3; i++)
        {
        	databaseList.add(new DatabaseAccessJDBC(String.valueOf(args[1])));
        }
        
		blackList = new MaliciousClients(databaseList);
        
		// set the delay for the blacklist's update from the database
		if (args.length == 3) {
			timer = new TimerUpdate(Integer.valueOf(args[2]));
		}
		else {
			timer = new TimerUpdate();
		}
		// add the update capacity to the blacklist
        blackList.setUpdateType(timer);
        
        while (true)
        {
        	// accept and process clients' requests
        	try 
        	{
				new ClientsFeedbackServer(serverSocket.accept(), blackList).start();
			} 
        	catch (IOException e) 
        	{
				e.printStackTrace();
			}
        }
     
	}

}
