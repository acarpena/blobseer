package util;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.nio.ByteBuffer;

/**
 * 
 * class that implements the methods for serializing objects and deserializing byte arrays
 */
public class ByteArrayUtil
{

	public static byte[] getBytes(Object obj) throws java.io.IOException
	{
		ByteArrayOutputStream bos = new ByteArrayOutputStream();
		ObjectOutputStream oos = new ObjectOutputStream(bos);
		oos.writeObject(obj);
		oos.flush();
		oos.close();
		bos.close();
		byte [] data = bos.toByteArray();
		return data;
	}
	
	public static Object deserialize(byte []data) throws IOException
	{
        Object result = null;
		ByteArrayInputStream bis = new ByteArrayInputStream(data);
		ObjectInputStream in = new ObjectInputStream(bis);
		try
		{
			result = in.readObject();
		} catch (ClassNotFoundException e)
		{
			e.printStackTrace();
		}
		in.close();
		bis.close();
		return result;
	}
	
	public static byte[] intToBytes(int i){
		ByteBuffer bb = ByteBuffer.allocate(4); 
		bb.putInt(i); 
		return bb.array();
		}
	public static int bytesToInt(byte[] intBytes){
		ByteBuffer bb = ByteBuffer.wrap(intBytes);
		return bb.getInt();
		}

}
