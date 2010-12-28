package mon.filters;

import java.util.HashMap;
import java.util.logging.*;

import lia.Monitor.JiniClient.Store.Filter;
import lia.Monitor.monitor.eResult;
import mon.filters.blobscreatemonitor.BlobsData;
import mon.filters.blobscreatemonitor.BlobsTable;
import mon.filters.providermonitor.ProviderData;
import mon.filters.providermonitor.ProviderReadTable;
import mon.filters.providermonitor.ProviderWriteTable;
import mon.util.db.DatabaseAccess;
import mon.util.db.DatabaseFastDB;
import mon.filters.vmanagermonitor.VManagerData;
import mon.filters.vmanagermonitor.VManagerWriteTable;


public class BSFilter implements Filter
    {
		
		private static final Logger fLogger =Logger.getLogger(BSFilter.class.getPackage().getName());
		
		private HashMap<String, Boolean> initialized_blobs;
		private Boolean firstBlob;
		/**
		 * initialized_blobs - each element in the HashMap corresponds to a created blob
		 * firstBlob - is true if for at least one blob a Create message was received
		 */
        
        
		public BSFilter()
        {      	
        	initialized_blobs = new HashMap<String, Boolean>();
        	firstBlob = false;
        	
        	 try {
        		 	// Log the filtered parameters into a file
        	        FileHandler handler = new FileHandler("filter.log");
        	        handler.setFormatter(new SimpleFormatter());

        	        fLogger.addHandler(handler);
        	    } catch (Exception e) {
        	    }
        }
        
		
		/**
		 *  Initializes the data base and the objects that deal with every kind of table 
		 *  (writes and reads received from the provider, writes from the vmanager)
		 */
		public Object filterData(Object data)
        {
			if (!(data instanceof eResult))
            {
                return null;
            }
            if (data==null)
            {
                return null;
            }
           
            final eResult r = (eResult) data;
            final DatabaseAccess db = new DatabaseFastDB();
            final ProviderWriteTable write = new ProviderWriteTable(db);
            final ProviderReadTable read = new ProviderReadTable(db);
            final VManagerWriteTable vmWrite = new VManagerWriteTable(db);
            
            if (r.ClusterName.equals("Blob_IO"))
            {
            	processBlobIO(r, db, write, read, vmWrite);
            	return null;
            }
               
            if (r.ClusterName.equals("VManager"))
	        {
	        	processVManagerData(r, db, write, read, vmWrite);
	        	return null;
	        }
            
             
            return null;
        }
        
        /**
         * 
         * 
         * @param r - the received parameter
         * @param db - the database handler
         * @param write - the object that deals with accessing the Provider writes
         * @param read- the object that deals with accessing the Provider reads
         * @param vmWrite- the object that deals with accessing the VManager writes
         */
        private void processVManagerData(eResult r, DatabaseAccess db, ProviderWriteTable write, 
        								 ProviderReadTable read, VManagerWriteTable vmWrite) {
        	final BlobsTable blobList = new BlobsTable(db);
            
            for (int i=0; i<r.param_name.length; i++)
            {
                String sParam = r.param_name[i];
                
                String[] IO_arr=((String) r.param[i]).split("#");
                
                if (sParam.equals("blobWrite"))
                {
                	VManagerData vman = new VManagerData(IO_arr);
                	initializeTables(vman.blob_id, read, write, vmWrite);
                    vmWrite.insert(vman);
                }
                
                if (sParam.equals("blobCreate"))
                {
                	BlobsData bdata = new BlobsData(IO_arr);
                	initializeBlobs(blobList);
			initializeTables(bdata.blob_id, read, write, vmWrite);
                	blobList.insert(bdata);
                }
                
            }
			
		}

		/**
		 * 
		 * @param r
		 * @param db
		 * @param write
		 * @param read
		 * @param vmWrite
		 */
		private void processBlobIO(eResult r, DatabaseAccess db, ProviderWriteTable write, 
        							ProviderReadTable read, VManagerWriteTable vmWrite) {
			
            for (int i=0; i<r.param_name.length; i++)
            {
                String sParam = r.param_name[i];
                
                String[] IO_arr=((String) r.param[i]).split("#");
                ProviderData prov = new ProviderData(r.FarmName, r.NodeName, IO_arr);
                
                if (sParam.equals("provider_write"))
                {
                 	initializeTables(prov.blob_id, read, write, vmWrite);
                    write.insert(prov);
                }
                
                if (sParam.equals("provider_read"))
                {               	
                	initializeTables(prov.blob_id, read, write, vmWrite);
                	read.insert(prov);
                }
                
            }
			
		}

		
		/**
		 * 
		 * @param blob_id
		 * @param read
		 * @param write
		 * @param vmWrite
		 */
		private void initializeTables(String blob_id, ProviderReadTable read, 
									ProviderWriteTable write, VManagerWriteTable vmWrite)
        {
        	if (initialized_blobs.get(blob_id) == null)
            {
            	synchronized (initialized_blobs)
                {
                	Boolean isCreated = initialized_blobs.get(blob_id);
                	if (isCreated == null)
                	{
                		fLogger.finest("Creating tables for blob" +blob_id );
                		
                		write.createTable(blob_id);
                		read.createTable(blob_id);
                		vmWrite.createTable(blob_id);
                		initialized_blobs.put(blob_id, true);
                	}
                }
            }
        }
		
		
		/**
		 * 
		 * @param blobList - the object that deals with the database accesses related to BLOBS table
		 */
	    private void initializeBlobs(BlobsTable blobList) {
	    	
	    	if (!firstBlob.booleanValue())
            {
            	synchronized (firstBlob)
                {
                	if (!firstBlob.booleanValue())
                	{
                		fLogger.finest("Creating the BLOBS table");
                		
                		blobList.createTable("");
                		firstBlob = true;
                	}
                }
            }
				
	    }

        
    }

