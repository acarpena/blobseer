package processing.update;

import java.util.LinkedList;
import java.util.List;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;

import processing.tables.ProviderWriteTable;
import processing.tables.VManagerWriteTable;

import util.db.DatabaseAccess;
import util.db.DatabaseAccessJDBC;
import data.MonitorData;
import data.UpdatedClientsInfo;

//the class in which we store monitoring data into the database
public class ClientDBUpdate implements Runnable 
{
	private DatabaseAccess db;
	private Vector<String> provider_clients_list; //the clients for which we store information on this node
	VManagerWriteTable table;
	private ConcurrentHashMap<String, UpdatedClientsInfo> updated_clients;
	
	private List<MonitorData> queue = new LinkedList<MonitorData>();
		
	/**
	 * Thread that receives and processes information received from the MonALISA filter
	 * @param data
	 * @param updated_clients 
	 * HashMap that is shared by both this thread and the one that summarizes 
	 * received information. Its purpose is to mark the clients for which there have been received updates.
	 * The summary thread unmarks them after treating the new information.
	 */
	public ClientDBUpdate( String propFile, ConcurrentHashMap<String, UpdatedClientsInfo> updated_clients)
	{
		provider_clients_list = new Vector<String>();
		db = new DatabaseAccessJDBC(propFile); 
		table = new VManagerWriteTable(db);
		table.createTable(null);
		this.updated_clients = updated_clients;
	}
		
	private void updateProviderInfo(MonitorData monitorData)
	{
		ProviderWriteTable table = new ProviderWriteTable(db);
		if (!provider_clients_list.contains(monitorData.clientID)) 
		{
			table.createTable(monitorData.clientID);
			provider_clients_list.add(monitorData.clientID);
		}
		table.insert(monitorData);
		if (updated_clients.containsKey(monitorData.clientID)) //if it alread exists it may be with the vmanager
		{
			UpdatedClientsInfo client_info = updated_clients.get(monitorData.clientID);
			client_info.addUpdated(UpdatedClientsInfo.PROVIDER_INFO);
			
			//put back updated info
			updated_clients.put(monitorData.clientID, client_info);
		}
		else //put new entry
			updated_clients.put(monitorData.clientID, new UpdatedClientsInfo(UpdatedClientsInfo.PROVIDER_INFO));
		
	}
	
	private void updateVManagerInfo(MonitorData monitorData)
	{
		table.insert(monitorData);
		if (updated_clients.containsKey(monitorData.clientID))
		{
			UpdatedClientsInfo client_info = updated_clients.get(monitorData.clientID);
			client_info.addUpdated(UpdatedClientsInfo.VMANAGER_INFO);
			
			//put back updated info
			updated_clients.put(monitorData.clientID, client_info);
		}
		else //put new entry
			updated_clients.put(monitorData.clientID, new UpdatedClientsInfo(UpdatedClientsInfo.VMANAGER_INFO));
		
	}
	
	private void updateClientInfo(MonitorData monitorData)
	{
		//modify the timestamp with which data arrives 
		monitorData.updateTimestamp();
		if (monitorData.getMonDataType().equals(MonitorData.PROVIDER_STRING))
			updateProviderInfo(monitorData);
		else
			updateVManagerInfo(monitorData);
	}
	
	public void processData(MonitorData data)
	{   //worker
		MonitorData dataCopy = data.clone();
		
		synchronized(queue) 
		{
			queue.add(dataCopy);
			queue.notify();
		}
	}
	
	public void run() 
	{
		MonitorData dataEvent;
		
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
				dataEvent = (MonitorData) queue.remove(0);
			}
			
			//do processing
			updateClientInfo(dataEvent);
		}
	}
	
}
