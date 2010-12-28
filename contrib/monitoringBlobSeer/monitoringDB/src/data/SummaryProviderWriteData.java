//
//  SummaryProviderWriteData.java
//  filters
//
//  Created by Alexandra Carpen-Amarie on 23/5/10.
//
package data;

/**
 * encapsulates the monitoring information summarized from the information sent by the blobseer providers
 * 
 */
public class SummaryProviderWriteData extends MonitorData
{
    /**
	 * 
	 */
	private static final long serialVersionUID = 534270918834772712L;
	public String blob_id;
    public String watermark;
    public long pagescount;
    public long page_size;
    // the first and the last timestamp of the recorded data
    public String firstdate;
    public String lastdate;
 
    public SummaryProviderWriteData()
    {
    	clientID = new String();
    	timestamp = new String();
    	blob_id = new String();
    	watermark = new String();
    	pagescount = 0;
    	page_size = 0;
    	firstdate = new String();
    	lastdate = new String();
    }
      
    public SummaryProviderWriteData(SummaryProviderWriteData pm)
    {
        super(pm.timestamp, pm.clientID);
        
        blob_id = pm.blob_id;
    	watermark = pm.watermark;
    	pagescount = pm.pagescount;
    	page_size = pm.page_size;
    	firstdate = pm.firstdate;
    	lastdate = pm.lastdate;
    }

	@Override
	public int getMonDataType()
	{
		return MonitorData.PROVIDER_WRITE_SUMMARY;
	}

}
