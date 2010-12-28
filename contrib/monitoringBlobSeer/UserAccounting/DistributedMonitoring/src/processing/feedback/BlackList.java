package processing.feedback;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.channels.SocketChannel;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Vector;
import java.util.concurrent.TimeoutException;

import nio.server.NioServer;
import nio.server.ServerWorker;
import processing.NodeCommand;
import processing.timers.TimerUpdate;
import util.ByteArrayUtil;
import util.NodeInfo;
import data.ClientHashData;
import data.clientTypes.IncorrectWrites;
import data.clientTypes.NormalOperations;
import data.clientTypes.PublishNoWrite;
import data.clientTypes.WriteNoPublish;

/**
 * class that gathers all the malicious clients and computes a black list <br>
 * uses a blocking client behaviour to request the users' list from the processing nodes <br>
 * uses the nio server to receive connections from clients requesting the list
 * @author Michi
 *
 */
public class BlackList implements ServerWorker
{
	private HashMap<String, ClientHashData> clientData ; //indexed by client_id
	private Vector<BlackListElement> blackList; //the list of malicious clients
	private Vector<NodeInfo> nodes;
	private final int PORT; 
	private final int TIMEOUT;
	
	public BlackList(int port, String configFile, int timeout)
	{
		clientData = new HashMap<String, ClientHashData>();
		blackList = new Vector<BlackListElement>();
		PORT = port;
		TIMEOUT = timeout*1000; //in ms
		readNodes(configFile);
	}
	
	public void updateBlackList() 
	{
		try
		{
			requestResults();
		} catch (IOException e)
		{
			e.printStackTrace();
		}
		
		synchronized (blackList)
		{
			//empty the list
			blackList.clear();

			//iterate over the map and compute black list elements 
			Iterator<String> it = clientData.keySet().iterator();
			while (it.hasNext())
			{
				String key = it.next();
				ClientHashData data = clientData.get(key);
				BlackListElement element = computeBlackListElement(data);
				blackList.add(element);
			}
		}
	}
	
	private BlackListElement computeBlackListElement(ClientHashData data)
	{
		int score; 
		score = data.getIw().getIWNumber()+data.getPnw().getPNWNumber()+data.getWnp().getWNPNumber();
		ClientScoreListElement element = new ClientScoreListElement(data.clientID);
		element.setScore(score);
		return element;
	}
	
	/**
	 * requests the lists of malicious users from every node
	 * @throws TimeoutException 
	 */
	private void requestResults() throws IOException
	{
		clientData.clear();
		for (NodeInfo node: nodes)
		{
			//connects to each node and asks for a result
			HashMap<String, ClientHashData> data = getResult(node);
			if (data != null)
				addToList(data);
		}
	}
	
	@SuppressWarnings("unchecked")
	private HashMap<String, ClientHashData> getResult(NodeInfo node)
	{
		NodeCommand command = new NodeCommand(NodeCommand.GATHER_CLIENT_INFO, null);
		Socket socket = null;  
		DataOutputStream os = null;
		DataInputStream is = null;
		boolean read_data = false;
		byte[] buffer = new byte[8192];
		HashMap<String, ClientHashData> result = null;

		try 
		{
			socket = new Socket();
			
			InetAddress addr = InetAddress.getByName(node.getNodeName());
			InetSocketAddress sockaddr = new InetSocketAddress(addr, node.getPort()); 
			
			socket.connect(sockaddr, TIMEOUT);
			socket.setSoTimeout(TIMEOUT);
			os = new DataOutputStream(socket.getOutputStream());
			is = new DataInputStream(socket.getInputStream());
			
		} 
		catch (UnknownHostException e) 
		{
			System.err.println("Don't know about host: "+node.getNodeName()+":"+node.getPort());
		} 
		catch (IOException e) 
		{
			System.err.println("Couldn't get I/O for the connection to: "+node.getNodeName()+":"+node.getPort());
		}
		
		// If everything has been initialized then we want to write some data
		// to the socket we have opened a connection to on port 
		if (socket != null && os != null && is != null) 
		{
			try 
			{
				os.write(ByteArrayUtil.getBytes(command));
				is.read(buffer);
				read_data = true;
				// clean up
				os.close();
				is.close();
				socket.close();   
			} 
			catch (UnknownHostException e) 
			{
				System.err.println("Trying to connect to unknown host: " + e);
				read_data = false;
			} 
			catch (IOException e) 
			{
				System.err.println("IOException:  " + e);
				read_data = false;
			}

		}
		if ( read_data)
		{
			try
			{
				result = (HashMap<String, ClientHashData>) ByteArrayUtil.deserialize(buffer);
			} catch (IOException e)
			{
				e.printStackTrace();
			}
		}
		
		return result;
	}
	
	private void addToList(HashMap<String, ClientHashData> hash_data)
	{
		Iterator<String> it = hash_data.keySet().iterator();
		while (it.hasNext())
		{
			String key = it.next();
			if (clientData.containsKey(key))
			{
				//updates existing data
				
				ClientHashData data = clientData.get(key);
				ClientHashData add_data = hash_data.get(key);
				updateIW(data, add_data);
				updateNO(data, add_data);
				updatePNW(data, add_data);
				updateWNP(data, add_data);
				clientData.put(key, data);
			}
			else
			{
				//introduces new entry
				clientData.put(key, hash_data.get(key));
			}
		}
	}
	
	private void updateWNP(ClientHashData data, ClientHashData add_data)
	{
		WriteNoPublish updated_wnp = data.getWnp();
		WriteNoPublish new_wnp_data = add_data.getWnp();
		updated_wnp.addBytesWritten(new_wnp_data.getSizeWritten());
		updated_wnp.addProviderPages(new_wnp_data.getProviderPagesNo());
		updated_wnp.addWNPNumber(new_wnp_data.getWNPNumber());
	}
	
	private void updatePNW(ClientHashData data, ClientHashData add_data)
	{
		PublishNoWrite updated_pnw = data.getPnw();
		PublishNoWrite new_pnw_data = add_data.getPnw();
		updated_pnw.addBytesWritten(new_pnw_data.getSizeWritten());
		updated_pnw.addVmanagerPages(new_pnw_data.getVmanagerPagesNo());
		updated_pnw.addPNWNumber(new_pnw_data.getPNWNumber());
	}
	
	private void updateIW(ClientHashData data, ClientHashData add_data)
	{
		IncorrectWrites updated_iw = data.getIw();
		IncorrectWrites new_iw_data = add_data.getIw();
		updated_iw.addBytesWritten(new_iw_data.getSizeWritten());
		updated_iw.addVmanagerPages(new_iw_data.getVmanagerPagesNo());
		updated_iw.addProviderPages(new_iw_data.getProviderPagesNo());
		updated_iw.addIWNumber(new_iw_data.getIWNumber());
	}
	
	private void updateNO(ClientHashData data, ClientHashData add_data)
	{
		NormalOperations updated_no = data.getNormalOps();
		NormalOperations new_no_data = add_data.getNormalOps();
		updated_no.insertMultipleOps(new_no_data.getCorrectWritesNo(), new_no_data.getCorrectPagesNo(), new_no_data.getTotalBytesWritten());
	}
	
	private void readNodes(String fileName) 
	{
		nodes = new Vector<NodeInfo>();
		try 
		{
			//use buffering, reading one line at a time
			//FileReader always assumes default encoding is OK!
			BufferedReader input =  new BufferedReader(new FileReader(fileName));
			try {
				String line = null; //not declared within while loop
				
				while (( line = input.readLine()) != null)
				{
					String []str = line.split(" ");
					NodeInfo node_info;
					if (str.length >= 2)
					{
						node_info = new NodeInfo(str[0], Integer.parseInt(str[1]));
					}
					else
					{
						node_info = new NodeInfo(str[0], PORT);
					}
					nodes.add(node_info);
				}
			}
			finally {
				input.close();
			}
		}
		catch (IOException ex)
		{
			ex.printStackTrace();
		}
	}
	
	@SuppressWarnings("unchecked")
	public Vector<BlackListElement> getList() 
	{

		Vector<BlackListElement> retList = new Vector<BlackListElement>();

		synchronized (blackList)
		{
			retList = (Vector<BlackListElement>) blackList.clone();
		}
	
		return retList;
	}
	
	public static void main(String args[])
	{
		try
		{
			TimerUpdate tu = new TimerUpdate(10); //10 sec
			BlackList l = new BlackList(Integer.parseInt(args[0]),args[1], Integer.parseInt(args[2]));
			tu.setList(l);
			tu.startUpdating();
			new Thread(new NioServer(null, Integer.parseInt(args[3]), l)).start();
		} 
		catch (NumberFormatException e)
		{
			e.printStackTrace();
		} 
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}

	public void processData(NioServer server, SocketChannel socket,
			byte[] data, int count)
	{
		String command = null;
		
		try
		{
			command = (String) ByteArrayUtil.deserialize(data);
		} 
		catch (IOException e)
		{
			e.printStackTrace();
		}
		if (command == null)
			return;
		
		if (command.equals("GET LIST"))
		{
			//sends back the list
			try
			{
				server.send(socket, ByteArrayUtil.getBytes(getList()));
			} 
			catch (IOException e)
			{
				e.printStackTrace();
			}
		}
	}

	
	
}
