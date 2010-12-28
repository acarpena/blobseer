package processing.filters;

import java.util.Vector;

import lia.Monitor.monitor.eResult;
import lia.Monitor.Filters.GenericMLFilter;
import lia.Monitor.monitor.monPredicate;
import data.MonitorData;
import data.ProviderReadData;
import data.ProviderWriteData;

public class FilterProviderData extends GenericMLFilter{
	/**
	 * 
	 */
	private static final long serialVersionUID = 8157559775005218910L;
	private final static String Name = "BSproviderFilter";
	private static long TIMEOUT = 3000;
	private monPredicate[] predicates = null;
    private ProcessMonData dataManager;
     	
	public FilterProviderData( String farmName ) 
	{
        super( farmName );
        dataManager = new ProcessMonData();
        predicates = new monPredicate[]{new monPredicate(farmName, "Blob_IO", "*", -1, -1, null, null )};
	}
	
	
	protected MonitorData parseResult(eResult r)
	{
		String sParam = r.param_name[0]; 
        
        String[] IO_arr= ((String)r.param[0]).split("#");
        
        if (sParam.equals("provider_write"))
        {
        	 ProviderWriteData prov = new ProviderWriteData(r.FarmName, r.NodeName, IO_arr);
        	 return prov;
        }
        if (sParam.equals("provider_read"))
        {
        	 ProviderReadData prov = new ProviderReadData(r.FarmName, r.NodeName, IO_arr);
        	 return prov;
        }
        return null;
        
	}
     
	@Override
	public monPredicate[] getFilterPred()
	{
		return predicates;
	}

	@Override
	public String getName()
	{
		return Name;
	}

	@Override
	public Object expressResults()
	{
		return dataManager.expressResults();
	}
	

	@Override
	public void notifyResult(Object o) //called for every match inside the filter
	{
		if (o == null) return;
		if ( o instanceof Vector<?> ) {
			Vector<?> v = (Vector<?>) o;
			for ( int i = 0; i < v.size(); i++ ) 
			{
				notifyResult( v.elementAt( i ) );
			}

		} 
		else if (o instanceof eResult)
		{
			eResult r = (eResult) o;

			MonitorData received_param = (MonitorData)parseResult(r);
			dataManager.processData(received_param);
		}
	}

	@Override
	public long getSleepTime() {
		return TIMEOUT;
	}
}


