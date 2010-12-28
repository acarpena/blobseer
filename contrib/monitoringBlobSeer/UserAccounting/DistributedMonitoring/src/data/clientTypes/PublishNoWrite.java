package data.clientTypes;

/**
 * @see {@link IncorrectWrites}  class
 *  
 * @author Michi
 */
@SuppressWarnings("serial")
public class PublishNoWrite extends MaliciousClient
{
	private int pnw_number;
	
	public PublishNoWrite()
	{
		super();
		pnw_number = 0;
	}
	
	public void addPNWNumber(int pnw_no)
	{
		pnw_number += pnw_no;
	}
	
	public void increasePNWNumber()
	{
		pnw_number++;
	}
	
	public boolean isPNW()
	{
		return (pnw_number > 0);
	}
	
	public int getPNWNumber()
	{
		return pnw_number;
	}
	
	public void addVmanagerPages(int pages_no)
	{
		super.addVmanagerPages(pages_no);
	}
	
	public void addBytesWritten(long bytes_no)
	{
		super.addBytesWritten(bytes_no);
	}

	public PublishNoWrite clone()
	{
		PublishNoWrite result = new PublishNoWrite();
		super.clone(result);
		result.pnw_number = pnw_number;
		return result;
	}
}
