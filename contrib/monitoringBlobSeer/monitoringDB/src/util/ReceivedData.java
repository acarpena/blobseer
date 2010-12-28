package util;

public class ReceivedData {

	private int len;
	private byte[] data;
	private boolean dataType;
	
	public static final boolean TYPE_LENGTH= false;
	public static final boolean TYPE_DATA= true;
	
	public ReceivedData()
	{
		len = 0;
		data = new byte[0];
		dataType = TYPE_LENGTH;		// the first byte array will begin with the lenth
	}
	
	public ReceivedData(int l, byte[] d)
	{
		len = 0;
		data = new byte[l];
		System.arraycopy(d, 0, data, 0, l);
		dataType = TYPE_LENGTH;		// the first byte array will begin with the lenth
	}
	
	public byte[] consumeBytes(int l)
	{
		if (data.length < l)
		{
			return null;
		}
		
		byte[] b = new byte[l];
		System.arraycopy(data, 0, b, 0, l);
		
		byte[] newData = new byte[data.length-l];
		System.arraycopy(data, l, newData, 0, data.length-l);
		
		data = newData;
				
		return b;
	}
	
	public void addBytes(int l, byte[] b)
	{
		byte[] newData = new byte[data.length+l];
		System.arraycopy(data, 0, newData, 0, data.length);
		System.arraycopy(b, 0, newData, data.length, l);
		
		data = newData;
		
	}

	public boolean enoughData()
	{
		return (len<=data.length);
	}
	
	public int getLen() {
		return len;
	}

	public void setLen(int l) {
		len = l;
	}
	
	public byte[] getData() {
		return data;
	}
	
	public boolean getDataType()
	{
		return dataType;
	}
	
	public void setDataType(boolean val)
	{
		dataType = val;
	}
	
}
