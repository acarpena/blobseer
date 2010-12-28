package processing;

import java.io.IOException;
import java.nio.channels.SocketChannel;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Timer;
import java.util.concurrent.ConcurrentHashMap;

import nio.server.NioServer;
import nio.server.ServerDataEvent;
import nio.server.ServerWorker;
import processing.update.ClientDBUpdate;
import processing.update.ClientHashUpdate;
import processing.update.ClientSummaryUpdate;
import util.ByteArrayUtil;
import data.ClientHashData;
import data.MonitorData;
import data.UpdatedClientsInfo;

//aplicatia care ruleaza pe nod
public class NodeApp implements ServerWorker, Runnable
{
	private List<ServerDataEvent> queue = new LinkedList<ServerDataEvent>();
	private ClientDBUpdate client_dbu ;
	private ClientSummaryUpdate client_summary;
	private ClientHashUpdate client_hash;
		
	public NodeApp(String propFile, long delay)
	{
		//initialize client_dbu and client_hash;
		ConcurrentHashMap<String, UpdatedClientsInfo> updated_clients = new ConcurrentHashMap<String, UpdatedClientsInfo>();
		client_dbu = new ClientDBUpdate(propFile, updated_clients);
		client_summary = new ClientSummaryUpdate(propFile, updated_clients);
		client_hash = new ClientHashUpdate(propFile, 10*1000);
		
		//start the thread that stores into the database
		new Thread(client_dbu).start();
		//start summary timer
		Timer t1 = new Timer(); 
		t1.schedule(client_summary, delay*1000, delay*1000);
		//start hash timer
		Timer t2 = new Timer();
		t2.schedule(client_hash, delay*1000, delay*1000);
		
	}
	
	public void retrieveData(ServerDataEvent event)
	{
		byte[] data = event.data;
		NodeCommand command = null;
		try
		{
			command = (NodeCommand) ByteArrayUtil.deserialize(data);
		} catch (IOException e)
		{
			e.printStackTrace();
		}
		if (command == null)
			return;
		
		MonitorData monitorData = command.getMonitorData();
		switch (command.getCommand())
		{
			case NodeCommand.PROCESS_MONITOR_DATA:
				//send to clientDBUpdate 
				client_dbu.processData(monitorData);
				break;
			case NodeCommand.GATHER_CLIENT_INFO:
				//gets the hash map with data from the HashUpdate thread and sends it
				HashMap<String, ClientHashData> hash = new HashMap<String, ClientHashData>();
				
				client_hash.getMaliciousClients(hash);
				
				//send back a response containing data in the hash map 
				try
				{
					event.server.send(event.socket, ByteArrayUtil.getBytes(hash));
				} 
				catch (IOException e)
				{
					e.printStackTrace();
				}
				break;
		}	
	}
	
	
	public void processData(NioServer server, SocketChannel socket, byte[] data, int count) 
	{//worker
		byte[] dataCopy = new byte[count];
		System.arraycopy(data, 0, dataCopy, 0, count);
		synchronized(queue) 
		{
			queue.add(new ServerDataEvent(server, socket, dataCopy, count));
			queue.notify();
		}
	}
	
	public void run() 
	{
		ServerDataEvent dataEvent;
		
		while(true) 
		{
			// Wait for data to become available
			synchronized(queue) 
			{
				while(queue.isEmpty()) 
				{
					try 
					{
						queue.wait();
					} 
					catch (InterruptedException e) {}
				}
				dataEvent = (ServerDataEvent) queue.remove(0);
			}
			
			//do processing
			retrieveData(dataEvent);
		}
	}
	
	public static void main(String[] args) 
	{
		try 
		{
			NodeApp worker = new NodeApp(args[0], Long.parseLong(args[1]));
			new Thread(new NioServer(null, Integer.parseInt(args[2]), worker)).start();
			//infinte loop here
			worker.run();
		} 
		catch (IOException e) 
		{
			e.printStackTrace();
		}
	}
}
