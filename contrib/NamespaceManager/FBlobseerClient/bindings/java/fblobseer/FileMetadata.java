package fblobseer;

public class FileMetadata 
{
	private String path;
	private int type;
	private long blobId;
	
	public FileMetadata() {
		
	}
	
	public FileMetadata(String path, int type, long blobId) {
		this.path = path;
		this.type = type;
		this.blobId = blobId;
	}
	
	public void setPath(String p) {
		path = p;
	}
	
	public void setType(int t) {
		type = t;
	}
	
	public void setBlobId(long id) {
		blobId = id;
	}
	
	public String getPath() {
		return path;
	}
	
	public int getType() {
		return type;
	}
	
	public long getBlobId() {
		return blobId;
	}
}