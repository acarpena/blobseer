package data;

import java.io.Serializable;

import util.TimeUtil;

/**
 * generic monitoring data received from MonALISA
 * @author Michi
 *
 */
@SuppressWarnings("serial")
public abstract class MonitorData implements Serializable {
	
	public static final String PROVIDER_STRING="provider";
	public static final String VMANAGER_STRING="vmanager";
	public static final String CLIENT_HASH_STRING="client hash";
	public static final String CLIENT_SUMMARY_STRING="client summary";
	public static final String BLOBS_DATA="blobs data";
	public String timestamp; // collected data timestamp
	public String clientID; // currently the IP
	
	public MonitorData() {
		timestamp = TimeUtil.now();
		clientID = new String("127.0.0.1");
	}	
	
	public MonitorData(String clientID) {
		timestamp = TimeUtil.now();
		this.clientID = new String(clientID);
	}
	
	public MonitorData(String timestamp, String clientID) {
		this.timestamp = new String(timestamp);
		this.clientID = new String(clientID);
	}
    
	public void updateTimestamp()
	{
		this.timestamp = TimeUtil.now();
	}
	
	public abstract String getMonDataType();
	public abstract MonitorData clone();
	
}
