package data;

import java.io.Serializable;

import util.TimeUtil;

/**
 * generic monitoring data received from MonALISA
 * @author Michi
 *
 */
public abstract class MonitorData implements Serializable {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 2273536441792836644L;
	
	// types of monitoring data
	public static final int PROVIDER_WRITE = 0;
	public static final int PROVIDER_READ = 1;
	public static final int VMANAGER_WRITE = 2;
	public static final int VMANAGER_CREATE = 3;
	public static final int PMANAGER = 4;
	public static final int METADATA_PROV_NODES = 5;
	public static final int METADATA_PROV_PAGEID = 6;
	
	public static final int PROVIDER_WRITE_SUMMARY = 21;

	
	public static final int CLIENT_HASH_STRING=11;
	public static final int CLIENT_SUMMARY_STRING=12;
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
	
	public abstract int getMonDataType();
	
}
