package data;

/**
 * contains information about the updated state of another Object
 * we use it to signal which clients' information has been updated in the database
 * @author Michi
 *
 */
public class UpdatedClientsInfo
{
	private Boolean isUpdated;
	private long from_info = 0;
		
	public UpdatedClientsInfo(int type)
	{
		isUpdated = new Boolean(true);
		this.from_info = 1<<type;
	}
	
	public UpdatedClientsInfo()
	{
		isUpdated = new Boolean(true);
		this.from_info = 0;
	}
	
	
	public void setUpdated(int type)
	{
		isUpdated = Boolean.TRUE;
		this.from_info = 1<<type;
	}
	
	public void addUpdated(int type)
	{
		isUpdated = Boolean.TRUE;
		this.from_info |= 1<<type;
	}
	
	public boolean isTableUpdated(int type)
	{
		return ( ((1<<type) & this.from_info) != 0);
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
		

}



