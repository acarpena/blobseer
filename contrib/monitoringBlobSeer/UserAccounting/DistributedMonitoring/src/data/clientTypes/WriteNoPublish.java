package data.clientTypes;

/**
 * @see {@link IncorrectWrites} class
 * @author Michi
 *
 */
@SuppressWarnings("serial")
public class WriteNoPublish extends MaliciousClient
{
	private int wnp_number;
	
	public WriteNoPublish()
	{
		super();
		wnp_number = 0;
	}
	
	public void addWNPNumber(int wnp_no)
	{
		wnp_number += wnp_no;
	}
	
	public void increaseWNPNumber()
	{
		wnp_number++;
	}
	
	public int getWNPNumber()
	{
		return wnp_number;
	}
	
	public boolean isWNP()
	{
		return (wnp_number > 0);
	}
	public void addProviderPages(int pages_no)
	{
		super.addProviderPages(pages_no);
	}
	
	public void addBytesWritten(long bytes_no)
	{
		super.addBytesWritten(bytes_no); 
	}
	
	public WriteNoPublish clone()
	{
		WriteNoPublish result = new WriteNoPublish();
		super.clone(result);
		result.wnp_number = wnp_number;
		return result;
	}

}
