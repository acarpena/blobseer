package vmonitor;

public class VManagerCCB extends MonitorData {

	public long blobID;
	public long pageSize;
	public int repFactor;

	public VManagerCCB(String timestamp, String clientID, long blobID, long pageSize, int repFactor) {
		super(timestamp, clientID);
		this.blobID = blobID;
		this.pageSize = pageSize;
		this.repFactor = repFactor;
	}
	
	public String toString(){
		//prepare the string to be sent using ApMon
		return timestamp+"#"+clientID+"#"+blobID+"#"+pageSize+"#"+repFactor;  
	}

}
