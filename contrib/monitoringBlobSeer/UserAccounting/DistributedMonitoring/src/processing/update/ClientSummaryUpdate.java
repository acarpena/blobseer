package processing.update;

import java.util.HashMap;
import java.util.Iterator;
import java.util.TimerTask;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;

import data.ClientSummaryData;
import data.ProviderData;
import data.UpdatedClientsInfo;
import data.VManagerData;

import processing.tables.ClientSummaryTable;
import processing.tables.ProviderWriteTable;
import processing.tables.VManagerWriteTable;
import util.TimeUtil;
import util.db.DatabaseAccessJDBC;

// the class where we update the table with all the clients this node contains info about
public class ClientSummaryUpdate extends TimerTask
{
	private String last_timestamp;
	private ClientSummaryTable summary_table;
	private ProviderWriteTable provider_table;
	private VManagerWriteTable vmanager_table;
	private ConcurrentHashMap<String, UpdatedClientsInfo> updated_clients;
	
	public ClientSummaryUpdate(String propFile, ConcurrentHashMap<String, UpdatedClientsInfo> updated_clients)
	{
		//one database access per thread so we don't have to handle synchronizing access to the database
		provider_table = new ProviderWriteTable(new DatabaseAccessJDBC(propFile));
		vmanager_table = new VManagerWriteTable(new DatabaseAccessJDBC(propFile));
		summary_table = new ClientSummaryTable(new DatabaseAccessJDBC(propFile));
		summary_table.createTable(null); //no client id here 
		this.updated_clients = updated_clients;
		last_timestamp = TimeUtil.now();
	}
	
	private void updateSummary()
	{
		Iterator<String> iterator = updated_clients.keySet().iterator();
	
		while (iterator.hasNext())
		{
			String key = iterator.next();
			UpdatedClientsInfo clientsInfo = updated_clients.get(key);
			if (clientsInfo.isUpdated())
			{
				//create the hash map that contains information about all the writes that have been updated
				HashMap<String, ClientSummaryData> client_summary_hash = new HashMap<String,ClientSummaryData>();
				//do processing
				if (clientsInfo.isProviderInfo())
				{
					Vector<ProviderData> provider_data_vector = provider_table.getData(key,last_timestamp);
					//iterate over the vector with provider data
					for (ProviderData provider_data:provider_data_vector)
					{
						ClientSummaryData summary_data;
						String client_sum_hash_key = hashKey(provider_data.blob_id, provider_data.watermark, provider_data.page_size);
						if (client_summary_hash.containsKey(client_sum_hash_key))
						{
							summary_data = client_summary_hash.get(client_sum_hash_key);
							if (provider_data.page_index.equals("0")) //if == 0 then more pages from the same write arrived and we will check for them in the page_number field
							{
								summary_data.provider_pages += provider_data.page_number;
							}
							else
								summary_data.provider_pages ++; //one page arrived - increases by 1
							
							//vmanager side
							if (clientsInfo.isVmanagerInfo()) //if vman info too has been updated then we'll treat it there
							{
								//nothing here
							}
							else
							{
								summary_data.write_size_vman = 0;
							}
						}
						else
						{
							summary_data = new ClientSummaryData(provider_data.clientID, provider_data.blob_id,1 ,provider_data.watermark, 0, Integer.decode(provider_data.page_size));
							if (provider_data.page_index.equals("0")) 
							{
								summary_data.provider_pages = provider_data.page_number;
							}
							else
								summary_data.provider_pages = 1;
							
							summary_data.write_size_vman = 0;
						}
						//put it back
						client_summary_hash.put(client_sum_hash_key, summary_data);
					}
					
				}
				if (clientsInfo.isVmanagerInfo())
				{
					Vector <VManagerData> vmanager_data_vector = vmanager_table.getData(key,last_timestamp);
					for (VManagerData vmanager_data:vmanager_data_vector)
					{
						ClientSummaryData summary_data;
						String client_sum_hash_key = hashKey(vmanager_data.blob_id, vmanager_data.watermark, vmanager_data.page_size);
						if (client_summary_hash.containsKey(client_sum_hash_key))
						{
							summary_data = client_summary_hash.get(client_sum_hash_key);
							summary_data.write_size_vman += Long.parseLong(vmanager_data.writeSize);
						}
						else
						{
							summary_data = new ClientSummaryData(vmanager_data.clientID, vmanager_data.blob_id,0 ,vmanager_data.watermark, 0, Integer.decode(vmanager_data.page_size));
							summary_data.provider_pages = 0;
							summary_data.write_size_vman = Long.parseLong(vmanager_data.writeSize);
						}
						//put it back
						client_summary_hash.put(client_sum_hash_key, summary_data);
					}
				}
				//insert all data into summary table
				Iterator<String> it = client_summary_hash.keySet().iterator();
				while (it.hasNext())
					summary_table.insert((ClientSummaryData)client_summary_hash.get(it.next()));
				
				//signal this client's info has been treated
				clientsInfo.removeUpdated();
				updated_clients.put(key, clientsInfo);
			}
		}
		
		last_timestamp = TimeUtil.now();
	}
	
	private String hashKey(String blob_id, String watermark, String page_size)
	{
		String result = new String(blob_id+" "+watermark+" "+page_size);
		return result;
	}

	public void run()
	{
		//update
		updateSummary();
	}
}
