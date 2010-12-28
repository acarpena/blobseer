package data.clientTypes;

import java.io.Serializable;

/** contains data specific to a normal operation performed by a client
 * 
 * @author Michi
 *
 */
@SuppressWarnings("serial")
public class NormalOperations implements Serializable
{
	private int correct_writes_no;
	private int correct_pages_no;
	private int total_correct_bytes_written;
	
	public NormalOperations()
	{
		correct_pages_no = 0;
		correct_writes_no = 0;
		total_correct_bytes_written = 0;
	}
	
	public int getCorrectWritesNo()
	{
		return correct_writes_no;
	}
	
	public int getCorrectPagesNo()
	{
		return correct_pages_no;
	}
	
	public int getTotalBytesWritten()
	{
		return total_correct_bytes_written;
	}
	
	public void insertOperation(int pages_no,long bytes_no)
	{
		correct_writes_no++;
		correct_pages_no += pages_no;
		total_correct_bytes_written += bytes_no;
	}
	
	public void insertMultipleOps(int op_no, int pages_no, long bytes_no)
	{
		correct_writes_no += op_no;
		correct_pages_no += pages_no;
		total_correct_bytes_written += bytes_no;
	}
	
	public NormalOperations clone()
	{
		NormalOperations result = new NormalOperations();
		result.correct_pages_no = correct_pages_no;
		result.correct_writes_no = correct_writes_no;
		result.total_correct_bytes_written = total_correct_bytes_written;
		return result;
	}
}
