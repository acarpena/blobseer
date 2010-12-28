package data;

import data.clientTypes.IncorrectWrites;
import data.clientTypes.NormalOperations;
import data.clientTypes.PublishNoWrite;
import data.clientTypes.WriteNoPublish;

/**
 * contains the history information we choose to keep about each client after processing new monitoring data 
 */
@SuppressWarnings("serial")
public class ClientHashData extends MonitorData
{
	private IncorrectWrites iw;
	private PublishNoWrite pnw;
	private WriteNoPublish wnp;
	private NormalOperations normalOps;
	
	public ClientHashData()
	{
		super();
		iw = new IncorrectWrites();
		pnw = new PublishNoWrite();
		wnp = new WriteNoPublish();
		normalOps = new NormalOperations();
	}
		
	public ClientHashData(String clientID) 
	{
		super(clientID);
		iw = new IncorrectWrites();
		pnw = new PublishNoWrite();
		wnp = new WriteNoPublish();
		normalOps = new NormalOperations();
	}
	
	public ClientHashData(String timestamp, String clientID) 
	{
		super(timestamp,clientID);
		iw = new IncorrectWrites();
		pnw = new PublishNoWrite();
		wnp = new WriteNoPublish();
		normalOps = new NormalOperations();
	}
    
	
	public String getId()
	{
		return clientID;
	}
	
	@Override
	public String getMonDataType()
	{
		return CLIENT_HASH_STRING;
	}

	public IncorrectWrites getIw()
	{
		return iw;
	}

	public PublishNoWrite getPnw()
	{
		return pnw;
	}

	public WriteNoPublish getWnp()
	{
		return wnp;
	}

	public NormalOperations getNormalOps()
	{
		return normalOps;
	}

	@Override
	public MonitorData clone()
	{
		ClientHashData result = new ClientHashData();
		result.clientID = new String(clientID);
		result.timestamp = new String(timestamp);
		result.iw = (IncorrectWrites) this.iw.clone();
		result.pnw = (PublishNoWrite) this.pnw.clone();
		result.wnp = (WriteNoPublish) this.wnp.clone();
		result.normalOps = this.normalOps.clone();
		return result;
	}


}
