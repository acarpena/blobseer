package processing.update;

import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;

import processing.tables.MetadataProvNodesTable;
import processing.tables.MetadataProvReqPagesTable;
import processing.tables.PmanagerTable;
import processing.tables.ProviderReadTable;
import processing.tables.ProviderWriteTable;
import processing.tables.SummaryProvWrite;
import processing.tables.TableInterface;
import processing.tables.VManagerWriteTable;

import util.db.DatabaseAccess;
import util.db.DatabaseAccessJDBC;
import data.MonitorData;
import data.UpdatedClientsInfo;

//the class in which we store monitoring data into the database
public class ClientDBUpdate implements Runnable 
{
	private DatabaseAccess db;
	private HashSet<String> clients_list; //the clients for which we store information on this node
	
	ProviderWriteTable providerWriteTable;
	VManagerWriteTable vmanWriteTable;
	ProviderReadTable providerReadTable;
	PmanagerTable pmanagerTable;
	MetadataProvNodesTable metadataProvNodesTable;
	MetadataProvReqPagesTable metadataProvPageIdTable;
	
	private List<MonitorData> queue = new LinkedList<MonitorData>();
		
	private ConcurrentHashMap<String, UpdatedClientsInfo> updated_clients;
	
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
		clients_list = new HashSet<String>();
		db = new DatabaseAccessJDBC(propFile);
		this.updated_clients = updated_clients;
		
		// initialize objects that deal with each kind of table
		providerWriteTable = new ProviderWriteTable(db);
		vmanWriteTable = new VManagerWriteTable(db);
		providerReadTable = new ProviderReadTable(db);
		pmanagerTable = new PmanagerTable(db);
		metadataProvNodesTable = new MetadataProvNodesTable(db);
		metadataProvPageIdTable = new MetadataProvReqPagesTable(db);
	}
		
	
	private void createTables(String clientID)
	{
		providerWriteTable.createTable(clientID);
		vmanWriteTable.createTable(clientID);
		providerReadTable.createTable(clientID);
		pmanagerTable.createTable(clientID);
		metadataProvNodesTable.createTable(clientID);
		metadataProvPageIdTable.createTable(clientID);
		
		SummaryProvWrite summPW = new SummaryProvWrite(db);
		summPW.createTable(clientID);
	}
	
	private TableInterface getDataTable(int type)
	{
		switch (type) {
		case MonitorData.PROVIDER_WRITE: {
			return providerWriteTable;
		}
		case MonitorData.PROVIDER_READ: {
			return providerReadTable;
		}
		case MonitorData.PMANAGER: {
			return pmanagerTable;
		}
		case MonitorData.METADATA_PROV_NODES: {
			return metadataProvNodesTable;
		}
		case MonitorData.METADATA_PROV_PAGEID: {
			return metadataProvPageIdTable;
		}
		case MonitorData.VMANAGER_WRITE: {
			return vmanWriteTable;
		}
		}
		return null;
	}
	
	private void updateClientInfo(MonitorData monitorData)
	{
		//modify the timestamp with which data arrives 
		monitorData.updateTimestamp();
		
		if (!clients_list.contains(monitorData.clientID)) 
		{
			createTables(monitorData.clientID);
			clients_list.add(monitorData.clientID);
		}
		
		TableInterface table = getDataTable(monitorData.getMonDataType());
		if (table != null)
			table.insert(monitorData);

		modifyUpdatedClients(monitorData);
	}

	private void modifyUpdatedClients(MonitorData monitorData)
	{
		UpdatedClientsInfo client_info = null;

		if (updated_clients.containsKey(monitorData.clientID)) //if it already exists, just add new updates
		{
			client_info = updated_clients.get(monitorData.clientID);

			if (client_info != null)
				client_info.addUpdated(monitorData.getMonDataType());
			else
			{
				client_info = new UpdatedClientsInfo(monitorData.getMonDataType());
			}	
		}
		else //put new entry
			client_info = new UpdatedClientsInfo(monitorData.getMonDataType());

		//put back updated info
		updated_clients.put(monitorData.clientID, client_info);
	}
	
	
	public void processData(Vector<MonitorData> data)
	{   //worker
		
		synchronized(queue) 
		{
			queue.addAll(data);
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
