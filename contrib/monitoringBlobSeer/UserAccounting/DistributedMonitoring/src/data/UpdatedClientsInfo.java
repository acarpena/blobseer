package data;

/**
 * contains information about the updated state of another Object
 * we use it to signal which clients' information has been updated in the database
 * @author Michi
 *
 */
public class UpdatedClientsInfo
{
	public static final int PROVIDER_INFO = 1;
	public static final int VMANAGER_INFO = 2;
	private Boolean isUpdated;
	private int from_info = 0;
	
	public UpdatedClientsInfo(int from_info)
	{
		isUpdated = new Boolean(true);
		this.from_info = from_info;
	}
	
	public UpdatedClientsInfo(int from_info1, int from_info2)
	{
		isUpdated = new Boolean(true);
		this.from_info = from_info1 | from_info2;
	}
	
	
	public void setUpdated(int from_info)
	{
		isUpdated = Boolean.TRUE;
		this.from_info = from_info;
	}
	
	public void addUpdated(int from_info)
	{
		isUpdated = Boolean.TRUE;
		this.from_info |= from_info;
	}
	
	public void setUpdated(int from_info1, int from_info2)
	{
		isUpdated = Boolean.TRUE;
		this.from_info = from_info1 | from_info2;
	}

	public void removeUpdated()
	{
		isUpdated = Boolean.FALSE;
		from_info = 0;
	}
	
	public boolean isUpdated()
	{
		return isUpdated.booleanValue();
	}
	
	public boolean isProviderInfo()
	{
		return ( (from_info & UpdatedClientsInfo.PROVIDER_INFO) != 0);
	}
	
	public boolean isVmanagerInfo()
	{
		return ((from_info & UpdatedClientsInfo.VMANAGER_INFO) != 0);
	}
}
