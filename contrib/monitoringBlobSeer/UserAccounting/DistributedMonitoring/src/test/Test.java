package test;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Vector;

import processing.feedback.BlackListElement;
import processing.feedback.ClientScoreListElement;

import util.ByteArrayUtil;

/**
 * test class that connects to the black list module and prints the list
 * @author Michi
 *
 */
public class Test
{
	@SuppressWarnings("unchecked")
	public static void main(String args[])
	{
		Socket socket = null;  
		DataOutputStream os = null;
		DataInputStream is = null;
		byte[] buffer = new byte[8192];

		try 
		{
			socket = new Socket();
			InetAddress addr = InetAddress.getByName(args[0]);
			InetSocketAddress sockaddr = new InetSocketAddress(addr, Integer.parseInt(args[1])); 
			socket.connect(sockaddr);
			
			os = new DataOutputStream(socket.getOutputStream());
			is = new DataInputStream(socket.getInputStream());
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
		
		if (socket != null && os != null && is != null) 
		{
			try 
			{
				os.write(ByteArrayUtil.getBytes("GET LIST"));
				is.read(buffer);
			
				// clean up
				os.close();
				is.close();
				socket.close();   
				
				Vector<BlackListElement> result = (Vector<BlackListElement>) ByteArrayUtil.deserialize(buffer);
				System.out.println("Rezultate:");
				for (BlackListElement element:result)
				{
					ClientScoreListElement score_el = (ClientScoreListElement)element;
					System.out.println("client: "+score_el.getClientID()+" "+score_el.getScore());
				}
				
			} 
			catch (UnknownHostException e) 
			{
				System.err.println("Trying to connect to unknown host: " + e);
			} 
			catch (IOException e) 
			{
				System.err.println("IOException:  " + e);
			}

		}
	}
}
