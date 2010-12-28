package processing.update;

import java.util.HashMap;
import java.util.Iterator;
import java.util.TimerTask;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;

import data.MonitorData;
import data.SummaryProviderWriteData;
import data.UpdatedClientsInfo;
import processing.tables.ProviderWriteTable;
import processing.tables.SummaryProvWrite;
import util.db.DatabaseAccessJDBC;
import util.TimeUtil;

// the class where we update the table with all the clients this node contains info about
public class ClientSummaryUpdate extends TimerTask
{
	private SummaryProvWrite provWriteSummaryTable;
	private ProviderWriteTable providerWriteTable;
	
	private ConcurrentHashMap<String, UpdatedClientsInfo> updated_clients;
	private HashMap<String, String> timestamps;

	
	public ClientSummaryUpdate(String propFile, ConcurrentHashMap<String, UpdatedClientsInfo> updated_clients)
	{
		//one database access per thread so we don't have to handle synchronizing access to the database
		providerWriteTable = new ProviderWriteTable(new DatabaseAccessJDBC(propFile));
		provWriteSummaryTable = new SummaryProvWrite(new DatabaseAccessJDBC(propFile));
		
		this.updated_clients = updated_clients;
		timestamps = new HashMap<String, String>();
	}
	
	private void updateSummary()
	{
		Iterator<String> iterator = updated_clients.keySet().iterator();
	
		while (iterator.hasNext())
		{
			String clientId = iterator.next();
			
			System.out.println("Updating summary for client: "+ clientId);
			
			UpdatedClientsInfo clientInfo = updated_clients.get(clientId);
			if (clientInfo.isUpdated())
			{
				// for each type of monitoring data, check if the corresponding table was updated
				// and update the summary
				if (clientInfo.isTableUpdated(MonitorData.PROVIDER_WRITE))
				{
					Vector<SummaryProviderWriteData> summ;
					summ = providerWriteTable.getUpdates(clientId, timestamps.get(clientId + MonitorData.PROVIDER_WRITE));
					
					String latestTimestamp = TimeUtil.getInitTimestamp();
					for (SummaryProviderWriteData elem: summ)
					{
						provWriteSummaryTable.insert(elem);
						if ( TimeUtil.getTimeMillis(elem.timestamp) > TimeUtil.getTimeMillis(latestTimestamp) )
						{
							latestTimestamp = elem.timestamp;
						}
							
					}
					
					// update the latest timestamp for this client
					timestamps.put(clientId + MonitorData.PROVIDER_WRITE, latestTimestamp);
				}
				
				// all the summaries have been updated
				// delete the update information for the current client
				clientInfo.removeUpdated();
			}
		}
	}
	

	public void run()
	{
		//update
		updateSummary();
	}
}
