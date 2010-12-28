package feedback.userAccounting;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Vector;

import feedback.userAccounting.blackList.BasicBlackList;

public class ClientsFeedbackServer extends Thread
{
	private Socket socket = null;
	private BasicBlackList blackList;

    public ClientsFeedbackServer(Socket socket, BasicBlackList bl) 
    {
		super();
		this.socket = socket; 
		this.blackList = bl;
	}

    public void run() 
    {
		try {
		    PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
		    BufferedReader in = new BufferedReader(
					    new InputStreamReader(
					    socket.getInputStream()));
	
		    String request;
		    Vector<String> result;
		    MaliciousCientsProtocol protocol = new MaliciousCientsProtocol(blackList);
	
		    // read the clients' requests and from the socket and send back the results
		    while ((request = in.readLine()) != null) {
				result = protocol.processInput(request);
				
				if (result == null) {
					continue;
				}
				
				for (String s: result) {
					out.println(s);
				}			
				
				if (request.equals("END")) {
				    break;
			    }
			    out.close();
			    in.close();
			    socket.close();
		    }
	
		} 
		catch (IOException e) 
		{
		    e.printStackTrace();
		}
		finally {
			//close the socket
			try 
		     {
		       if (socket != null) socket.close(  );
		     }
		     catch (IOException e) {/*the client closed the socket*/}
		   }

    }

}
