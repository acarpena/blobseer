import java.io.IOException;
import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.util.*;

public class LogClient
{
      private String hostname;
      private String localHostname;
      private int port;
      private Socket clientSocket;
      private BufferedWriter out;
    
      public LogClient(String hostname, int port) throws IOException
      {
	    this.hostname = hostname;
	    this.port = port;
	    clientSocket = new Socket(hostname, port);
	    out = new BufferedWriter(new OutputStreamWriter(clientSocket.getOutputStream()));
	    if(out == null)
		  throw new IOException();
	    
	    InetAddress addr = InetAddress.getLocalHost();
	    // Get IP Address
	    byte[] ipAddr = addr.getAddress();
	    
	    // Get hostname
	    localHostname = addr.getCanonicalHostName();
      }
      
      public void logMessage(String mes) throws IOException 
      {
	    out.write(localHostname + ": " + mes + "\n");
	    out.flush();
      }
}
