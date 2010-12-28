package processing;

import java.io.IOException;
import java.nio.channels.SocketChannel;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Timer;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;

import nio.server.NioServer;
import nio.server.ServerDataEvent;
import nio.server.ServerWorker;
import processing.update.ClientDBUpdate;
import processing.update.ClientSummaryUpdate;
import util.ByteArrayUtil;
import util.ReceivedData;
import data.MonitorData;
import data.UpdatedClientsInfo;

//aplicatia care ruleaza pe nod
public class NodeApp implements ServerWorker, Runnable
{
	private List<ServerDataEvent> queue = new LinkedList<ServerDataEvent>();
	private ClientDBUpdate client_dbu ;
	private HashMap<SocketChannel, ReceivedData> receivedData;

	private ClientSummaryUpdate client_summary;
	
	public NodeApp(String propFile, long delay)
	{
		receivedData = new HashMap<SocketChannel, ReceivedData>();

		//initialize client_dbu and client_hash;
		ConcurrentHashMap<String, UpdatedClientsInfo> updated_clients = new ConcurrentHashMap<String, UpdatedClientsInfo>();
		client_dbu = new ClientDBUpdate(propFile, updated_clients);
		client_summary = new ClientSummaryUpdate(propFile, updated_clients);

		//start the thread that stores into the database
		new Thread(client_dbu).start();
		//start summary timer
		Timer t1 = new Timer(); 
		t1.schedule(client_summary, delay*1000, delay*1000);
	}

	@SuppressWarnings("unchecked")
	public void retrieveData(ServerDataEvent event)
	{
		byte[] data = event.data;
		NodeCommand command = null;
		ReceivedData currentData = receivedData.get(event.socket);

		if (currentData == null)	// first time data is received from this socket 
		{
			currentData = new ReceivedData();
			receivedData.put(event.socket, currentData);
		}

		//System.out.println("Adding bytes: len="+currentData.getLen()+"  size="+event.size);
		// add the received buffer
		currentData.addBytes(event.size, data);

		while (true)
		{
			try
			{
				if (currentData.getDataType() == ReceivedData.TYPE_LENGTH)	
				{
					byte[] bb = currentData.consumeBytes(4);		// remove the first 4 bytes - the length
					
					if (bb == null) 
					{
						//System.out.println("less than 4 bytes");
						break;
					}

					int length = ByteArrayUtil.bytesToInt(bb);

					currentData.setLen(length);
					currentData.setDataType(ReceivedData.TYPE_DATA);

					//System.out.println("Found TYPE LENGTH: len="+currentData.getLen()+"  size="+currentData.getData().length+ "  length="+length);

				}

				// if we received the at least length bytes => attempt a deserialization
				if (currentData.enoughData())
				{
					
					//System.out.println("Try to consume data: len="+currentData.getLen()+"  size="+currentData.getData().length);

					byte[] buf = currentData.consumeBytes(currentData.getLen());

					//System.out.println("Now deserializing for length " + currentData.getLen());
					command = (NodeCommand) ByteArrayUtil.deserialize(buf);

					// data was removed from the buffer => expect a length at he beginning now
					currentData.setDataType(ReceivedData.TYPE_LENGTH);
				}
				else {
					//System.out.println("Not enough bytes received" + currentData.getLen());
					break;
				}

			} catch (IOException e)
			{
				System.out.println("Deserialization failed: length = " + currentData.getLen());
				e.printStackTrace();
			}

			if (command == null)
			{
				System.out.println("Deserialization aborted");
				return;
			}

			// deserialization successful
			System.out.println("Deserialization successful");

			Vector<MonitorData> monitorData = (Vector<MonitorData>) command.getMonitorData();
			switch (command.getCommand())
			{
			case NodeCommand.PROCESS_MONITOR_DATA:
				//send to clientDBUpdate 
				client_dbu.processData(monitorData);
				break;
			case NodeCommand.GATHER_CLIENT_INFO:
				//gets the hash map with data from the HashUpdate thread and sends it
				//						HashMap<String, ClientHashData> hash = new HashMap<String, ClientHashData>();
				//						
				//						client_hash.getMaliciousClients(hash);
				//						
				//						//send back a response containing data in the hash map 
				//						try
				//						{
				//							event.server.send(event.socket, ByteArrayUtil.getBytes(hash));
				//						} 
				//						catch (IOException e)
				//						{
				//							e.printStackTrace();
				//						}
				break;
			}	
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
