import java.net.*;
import java.io.*;
public class repServer
{
	public static void main(String[] args)
	{
		int port = 3333;
		int left=6, right=2;
		PrintWriter out = null;
		BufferedReader in = null;
		ServerSocket serverSocket = null;
		Socket clientSocket = null;
		try
		{	
			serverSocket = new ServerSocket(port);
			System.out.println("Server Socket bound! ");	
			while(true)
			{
				clientSocket = serverSocket.accept(); 
				out = new PrintWriter(clientSocket.getOutputStream(),true);
				in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
				String inputLine, outputLine;
				while( (inputLine = in.readLine()) != null)
				{
					System.out.println(inputLine+">"+inputLine.length());
					inputLine = inputLine.trim();
					System.out.println(inputLine+">"+inputLine.length());
					if(inputLine.equals("bye"))
						break;
					else if(inputLine.equals("+"))
					{
						int res=left+right;
						out.println(""+left+inputLine+right+" = "+res);
					}
					else if(inputLine.equals("-"))
					{
						int res=left-right;
						out.println(""+left+inputLine+right+" = "+res);
					}
					else if(inputLine.equals("*"))
					{
						int res=left*right;
						out.println(""+left+inputLine+right+" = "+res);
					}
					else if(inputLine.equals("/"))
					{
						int res=left/right;
						out.println(""+left+inputLine+right+" = "+res);
					}
					else
						out.println("Nothing:["+inputLine+"]>"+inputLine.length());
				}
			}
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		finally
		{
			try
			{
				in.close();
				out.close();
				serverSocket.close();
				clientSocket.close();
			}
			catch(Exception e)
			{
				e.printStackTrace();
			}
		}
	}
}
