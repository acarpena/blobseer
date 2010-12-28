package mon.filters.util;



public class MonitorData {
	
	public String timestamp; // collected data timestamp
	public String clientID; // currently the IP
	
	public MonitorData() {
		timestamp = TimeUtil.now();
		clientID = "127.0.0.1";
	}	
	
	public MonitorData(String clientID) {
		timestamp = TimeUtil.now();
		this.clientID = clientID;
	}
	
	public MonitorData(String timestamp, String clientID) {
		this.timestamp = timestamp;
		this. clientID = clientID;
	}
    
}
