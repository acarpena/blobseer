package data.clientTypes;

/**
 * contains the specific data that we are interested in in case of an IW client
 * @author Michi
 * 
 */
@SuppressWarnings("serial")
public class IncorrectWrites extends MaliciousClient
{
	private int iw_number;
	
	public IncorrectWrites()
	{
		super();
		iw_number = 0;
	}
	
	public void addIWNumber(int iw_no)
	{
		iw_number += iw_no;
	}
	
	public void increaseIWNumber()
	{
		iw_number++;
	}
	
	public int getIWNumber()
	{
		return iw_number;
	}
	
	public boolean isIW()
	{
		return (iw_number > 0);
	}
	
	public void addProviderPages(int pages_no)
	{
		super.addProviderPages(pages_no);
	}
	
	public void addVmanagerPages(int pages_no)
	{
		super.addVmanagerPages(pages_no);
	}
	
	public void addBytesWritten(long bytes_no)
	{
		super.addBytesWritten(bytes_no); 
	}
	
	public IncorrectWrites clone()
	{
		IncorrectWrites result = new IncorrectWrites();
		super.clone(result);
		result.iw_number = iw_number;
		return result;
	}

}
