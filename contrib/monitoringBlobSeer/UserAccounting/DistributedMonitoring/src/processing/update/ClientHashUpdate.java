package processing.update;

import java.util.HashMap;
import java.util.Iterator;
import java.util.TimerTask;
import java.util.Vector;
import processing.tables.ClientSummaryTable;
import util.TimeUtil;
import util.db.DatabaseAccessJDBC;

import data.ClientHashData;
import data.ClientSummaryData;

// the class in which we update the client history - we keep it in a hash map that is unique per node
public class ClientHashUpdate extends TimerTask
{
	//asta e initializata o singura data
	private final HashMap<String, ClientHashData> clientData ; //indexed by client_id
	private ClientSummaryTable summary_table;
	private String last_timestamp;
	private String prev_timestamp;
	private long time_to_wait; //how long an entry is kept in waiting_ops
	private HashMap<String, ClientSummaryData> waiting_operations; //hash from client_id,blob+watermark
			
	public ClientHashUpdate(String propFile, long time_to_wait) //in ms
	{
		summary_table = new ClientSummaryTable(new DatabaseAccessJDBC(propFile));
		clientData = new HashMap<String, ClientHashData>();
		waiting_operations = new HashMap<String, ClientSummaryData>();
		last_timestamp = TimeUtil.now();
		prev_timestamp = TimeUtil.now();
		this.time_to_wait = time_to_wait; //the entries are kept in the cache before they are aged out
	}
	
	public void getMaliciousClients(HashMap<String, ClientHashData> malicious_clients_map)
	{
		//HashMap<String, ClientHashData> malicious_clients_map = new HashMap<String, ClientHashData>();
		
			HashMap<String, ClientHashData> clientDataCopy = null;
			synchronized (clientData)
			{
				clientDataCopy = cloneMap(clientData);
			}
			Iterator<String> it = clientDataCopy.keySet().iterator();
			while (it.hasNext())
			{
				String key = it.next();
				ClientHashData data = clientDataCopy.get(key);
				if (data.getIw().isIW() || data.getPnw().isPNW() || data.getWnp().isWNP())
				{
					malicious_clients_map.put(key, data);
				}
			}
		
		//return malicious_clients_map;
	}
	
	private static HashMap<String, ClientHashData> cloneMap(HashMap<String, ClientHashData> map)
	{
		if (map == null)
			return null;
		else
		{
			HashMap<String, ClientHashData> clone = new HashMap<String, ClientHashData>();

			Iterator< String> it = map.keySet().iterator();
			while (it.hasNext())
			{
				String key = it.next();
				ClientHashData mapData = map.get(key);
				ClientHashData mapDataClone = (ClientHashData) mapData.clone();
				clone.put(new String(key),mapDataClone);
			}
			return clone;
		}
	}
	
	private void updateHash()
	{
		//modify timestamp
		last_timestamp = TimeUtil.now();
		
		//select on the table
		Vector<ClientSummaryData> client_summary_vector = summary_table.getData( prev_timestamp);
		//last_timestamp is a reference time for which we determine if entry expired 
		for (ClientSummaryData client_summary_data:client_summary_vector)
		{
			String data_key = hashKey(client_summary_data.clientID, client_summary_data.blob_id, client_summary_data.watermark);
			//update waiting_ops
			//if waiting time has expired, we continue to update it, if it did, we add a new entry to the hash map
			if ((waiting_operations.containsKey(data_key)) && ((TimeUtil.getTimeMillis(last_timestamp, waiting_operations.get(data_key).timestamp)) < time_to_wait))
			{
				ClientSummaryData waiting_ops_data = waiting_operations.get(data_key);
				waiting_ops_data.provider_pages += client_summary_data.provider_pages;
				waiting_ops_data.write_size_vman += client_summary_data.write_size_vman;
				
				//put it back
				waiting_operations.put(data_key, waiting_ops_data);
			}
			else
			{
				client_summary_data.timestamp = TimeUtil.now(); 
				//insert new entry in waiting_ops
				waiting_operations.put(data_key, client_summary_data);
			}
		}
		//update hash map 
		
		//iterate over waiting_ops, see which have expired
		Iterator<String> it = waiting_operations.keySet().iterator();
		Vector<String> expired_operations = new Vector<String>();
		while (it.hasNext())
		{
			String key = it.next(); 
			ClientSummaryData waiting_ops_data = waiting_operations.get(key);
			if ((TimeUtil.getTimeMillis(last_timestamp, waiting_ops_data.timestamp)) > time_to_wait)
			{
				ClientHashData client_hash_data;
				//the entry has expired, let's handle it
				synchronized (clientData)
				{
					if (clientData.containsKey(waiting_ops_data.clientID)) //if an entry already exists for this client in the hash map
					{
						client_hash_data = (ClientHashData) (clientData.get(waiting_ops_data.clientID)).clone();
					}
					else
					{
						client_hash_data = new ClientHashData(waiting_ops_data.clientID);
					}
				}
				//do processing
				long size_written_provider = waiting_ops_data.provider_pages * waiting_ops_data.page_size;
				long size_written_vmanager = waiting_ops_data.write_size_vman;
				if (size_written_provider == size_written_vmanager)
				{
					//a normal operation
					client_hash_data.getNormalOps().insertOperation(waiting_ops_data.provider_pages, waiting_ops_data.write_size_vman);
				}
				else
				{
					//a malicious client of a sort
					if (size_written_vmanager == 0) //wnp
					{
						client_hash_data.getWnp().increaseWNPNumber();
						client_hash_data.getWnp().addProviderPages(waiting_ops_data.provider_pages);
						client_hash_data.getWnp().addBytesWritten(size_written_provider);
					}
					else
						if (size_written_provider == 0) //pnw
						{
							client_hash_data.getPnw().increasePNWNumber();
							client_hash_data.getPnw().addVmanagerPages((int)(size_written_vmanager/waiting_ops_data.page_size));
							client_hash_data.getPnw().addBytesWritten(size_written_vmanager);
						}
						else //iw
						{
							client_hash_data.getIw().increaseIWNumber();
							client_hash_data.getIw().addProviderPages(waiting_ops_data.provider_pages);
							client_hash_data.getIw().addVmanagerPages((int)(size_written_vmanager/waiting_ops_data.page_size));
							client_hash_data.getIw().addBytesWritten(Math.abs(size_written_provider - size_written_vmanager));
						}
				}

				//put it back
				synchronized (clientData)
				{
					clientData.put(waiting_ops_data.clientID, client_hash_data);
				}
				expired_operations.add(key); //mark for removing from waiting_ops after all the iterating is done
			}
			
		}
		for (String op_key:expired_operations) //remove
		{
			waiting_operations.remove(op_key);
		}
		
		prev_timestamp = TimeUtil.now();
	}
	
	private String hashKey(String client_id, String blob_id, String watermark)
	{
		String result = new String(client_id+" "+blob_id+" "+watermark+" ");
		return result;
	}
	
	public void run()
	{
		updateHash();
	}
	
}
