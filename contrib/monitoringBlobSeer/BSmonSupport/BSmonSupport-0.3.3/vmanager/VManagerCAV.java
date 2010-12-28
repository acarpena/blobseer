package vmonitor;

public class VManagerCAV extends MonitorData {

	public long version;
	public long blobID;
	public String watermark;
	public long writeOffset;
	public long writeSize;
	
	public VManagerCAV(String timestamp, String clientID, long version, long blobID, String watermark, long writeOffset, long writeSize) {
		super(timestamp, clientID);
		this.blobID = blobID;
		this.version = version;
		this.watermark = watermark;
		this.writeOffset = writeOffset;
		this.writeSize = writeSize;
		
	}
	
	public String toString(){
		//prepare the string to be sent using ApMon
		return timestamp+"#"+clientID+"#"+version+"#"+blobID+"#"+watermark+"#"+writeOffset+"#"+writeSize;  
	}

}
