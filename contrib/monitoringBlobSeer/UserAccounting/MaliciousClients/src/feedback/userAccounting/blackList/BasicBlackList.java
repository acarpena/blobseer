package feedback.userAccounting.blackList;

import java.util.Vector;

import org.apache.log4j.Logger;

import mon.util.blob.Blobs;
import mon.util.db.DatabaseAccess;


/**
 * @author acarpena
 *
 *	Generic class that defines a blacklist of malicious clients
 */
public abstract class BasicBlackList{
	
	public static  Logger logger;
	protected DatabaseAccess db;				
	protected Vector<BlackListElement> list;	// the blacklist
	protected PeriodicalUpdate update;			// the update method of the blacklist

	protected Blobs blobs;						// the blobs retrieved from the database

	// Constructor
	public BasicBlackList(DatabaseAccess db) {
		
		this.db = db;
		this.blobs = new Blobs(db);
		this.list = new Vector<BlackListElement>();
	}
	
	public BasicBlackList() {
		
		this.list = new Vector<BlackListElement>();
	}
	
	// set the method that will be used to update the blacklist from the database
	public void setUpdateType(PeriodicalUpdate p) {
		
		update = p;
		
		if (update != null) {
			update.setList(this);
			update.startUpdating();
		}		
	}
	
	// return the values in the blacklist
	@SuppressWarnings("unchecked")
	public Vector<BlackListElement> getList() {
		
		Vector<BlackListElement> retList = new Vector<BlackListElement>();
		
		synchronized(list) {	// prevent the list from being written in the same time by the updating module
			
			retList = (Vector<BlackListElement>) list.clone();
		}
		
		return retList;
	}
	
	
	public void setLogger(Logger log){
		logger = log;
	}
	
	
	
	// method that allows the list to be updated 
	public abstract void updateBlackList();
	
	

}
