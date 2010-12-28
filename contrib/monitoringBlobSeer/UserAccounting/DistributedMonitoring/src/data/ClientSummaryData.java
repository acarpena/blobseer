package data;

import util.TimeUtil;

/**
 * contains the information associated with a single client inside the summary layer
 * @author Michi
 *
 */
@SuppressWarnings("serial")
public class ClientSummaryData extends MonitorData //summary de la provider
{
    public String blob_id;
    public int provider_pages; //total number of pages written in this write
    public String watermark;
    public long write_size_vman;
    public int page_size;
    
    public ClientSummaryData()
    {
    	clientID = new String();
    	timestamp = TimeUtil.now();
    	blob_id = new String();
    	provider_pages = 0;
    	watermark = new String();
    	write_size_vman = 0;
    	page_size = 0;
    }
    
	public ClientSummaryData(String clientId, String blob_id, int provider_pages, String watermark, long write_size_vman, int page_size)
	{
		super(clientId);
		this.blob_id = new String(blob_id);
		this.provider_pages = provider_pages;
		this.watermark = new String(watermark);
		this.write_size_vman = write_size_vman;
		this.page_size = page_size;
	}
	
	@Override
	public String getMonDataType()
	{
		return CLIENT_SUMMARY_STRING;
	}

	@Override
	public MonitorData clone()
	{
		ClientSummaryData data = new ClientSummaryData( this.clientID,  blob_id,  provider_pages,  watermark,  write_size_vman,  page_size);
		return data;
	}

}
