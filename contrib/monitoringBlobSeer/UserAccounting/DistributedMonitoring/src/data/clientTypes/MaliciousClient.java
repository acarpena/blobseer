package data.clientTypes;

import java.io.Serializable;

/**
 * class that encapsulates information common to all malicious users
 * @author Michi
 *
 */
@SuppressWarnings("serial")
public abstract class MaliciousClient implements Serializable
{
	private int total_pages_written_to_provider;
	private int total_pages_written_to_vmanager;
	private long total_incorrect_bytes_written;
	
	public MaliciousClient()
	{
		total_pages_written_to_provider = 0;
		total_pages_written_to_vmanager = 0;
		total_incorrect_bytes_written = 0;
	}
	
	public int getProviderPagesNo()
	{
		return total_pages_written_to_provider;
	}
	
	public int getVmanagerPagesNo()
	{
		return total_pages_written_to_vmanager;
	}
	
	public long getSizeWritten()
	{
		return total_incorrect_bytes_written;
	}
	
	protected void addProviderPages(int pages_no)
	{
		total_pages_written_to_provider += pages_no;
	}
	
	protected void addVmanagerPages(int pages_no)
	{
		total_pages_written_to_vmanager += pages_no;
	}
	
	protected void addBytesWritten(long bytes_no)
	{
		total_incorrect_bytes_written += bytes_no; 
	}
	
	public void clone(MaliciousClient result)
	{
		result.total_incorrect_bytes_written = total_incorrect_bytes_written;
		result.total_pages_written_to_provider = total_pages_written_to_provider;
		result.total_pages_written_to_vmanager = total_pages_written_to_vmanager;
	}
	
}
