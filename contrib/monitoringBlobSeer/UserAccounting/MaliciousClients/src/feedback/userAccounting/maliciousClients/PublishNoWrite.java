package feedback.userAccounting.maliciousClients;

import java.util.HashMap;

import mon.util.db.DatabaseAccess;

import feedback.userAccounting.blackList.BasicBlackList;


/**
 * @author acarpena
 *
 *  Class implementing a black list of clients that publish writes to the version manager,
 *   but don't actually write anything on the data providers
 */
public class PublishNoWrite extends BasicBlackList {

	HashMap<String, Integer> publishedWrites;	// the number of the published writes that have no 
												// corresponding writes on the providers (for each client)
	
	public PublishNoWrite(DatabaseAccess db) {
		super(db);
	}

	
	public void updateBlackList() {
		
		gatherData();
		
		synchronized (list) {
			list.clear();
			
			for (String el:publishedWrites.keySet()) {
				list.add(new ClientScoreListElement(el, publishedWrites.get(el)));
			}
		}
		
		logger.info(list.toString());
	}
	
	
	protected void gatherData() {
		
		publishedWrites = new HashMap<String, Integer>();
		
		// find the clients that published inexistent writes for each blob
		for (String blobID : blobs.getBlobIDs()) {
			blobQuery(blobID);

			while (db.next()) {
				// retrieve the current number of writes
				Integer noOfPublishedWrites = publishedWrites.get(db.getString("client"));

				// initialize the number of published writes if it is the first
				// time the selected client is put into the list
				if (noOfPublishedWrites == null) {
					noOfPublishedWrites = 0;
				}

				// add the number of published writes for the current blob to
				// the current value
				noOfPublishedWrites += Integer.valueOf(db.getString("no"));
				// update the value for he current client
				publishedWrites.put(db.getString("client"), noOfPublishedWrites);
			}
		}
	}
	
	protected void blobQuery(String blobID){
		
		// search for the clients that published writes, 
		//but the watermark of their writes cannot be found in the received data-writes tables
		db.executeQuery("SELECT publishedWrites.client_id AS client, count(publishedWrites.watermark) AS no " +
				"FROM client_writes_" + blobID + " AS publishedWrites " +
				"WHERE NOT EXISTS " +
					"(" +
					"SELECT writes.watermark " +
					"FROM write_" + blobID + " AS writes " +
					"WHERE writes.watermark = publishedWrites.watermark" +
 					") " +
 				"GROUP BY publishedWrites.client_id");
	}

	
	
}
