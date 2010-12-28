package processing.filters;

import java.util.Vector;

import lia.Monitor.monitor.eResult;
import lia.Monitor.Filters.GenericMLFilter;
import lia.Monitor.monitor.monPredicate;
import data.MonitorData;
import data.VManagerData;

public class FilterVmanagerData extends GenericMLFilter{
	/**
	 * 
	 */
	private static final long serialVersionUID = -1448255747287518982L;
	private final static String Name = "BSvmanagerFilter";
	private static long TIMEOUT = 3000;
	private monPredicate[] predicates = null;
    private ProcessMonData dataManager;

     	
	public FilterVmanagerData( String farmName ) 
	{
        super( farmName );
        dataManager = new ProcessMonData();
        predicates = new monPredicate[]{new monPredicate(farmName, "VManager", "*", -1, -1, null, null )};
	}
	
	public VManagerData parseResult(eResult r)
	{
		String sParam = r.param_name[0]; 
        
        String[] IO_arr= ((String)r.param[0]).split("#");
        
        if (sParam.equals("blobWrite"))
        {
        	 VManagerData param = new VManagerData(IO_arr);
        	 return param;
        }
        if (sParam.equals("blobCreate"))
        {
        	 VManagerData param = new VManagerData(IO_arr);
        	 return param;
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
