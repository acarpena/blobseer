package feedback.userAccounting.maliciousClients;

import java.util.HashMap;

import mon.util.db.DatabaseAccess;

import feedback.userAccounting.blackList.BasicBlackList;

/**
 * @author acarpena
 *
 *  Class implementing a black list of clients that perform writes on the providers,
 *  without publishing them to the version manager
 */
public class WriteNoPublish extends BasicBlackList {

	HashMap<String, Double> unpublishedWrites;	// the number of the published writes that have no 
												// corresponding writes on the providers (for each client)
	
	public WriteNoPublish(DatabaseAccess db) {
		super(db);
		
	}


	public void updateBlackList() {
		
		gatherData();
		
		synchronized (list) {
			list.clear();
			
			for (String el:unpublishedWrites.keySet()) {
				list.add(new ClientScoreListElement(el, unpublishedWrites.get(el).intValue()));
			}
		}
		
		logger.info(list.toString());
	}
	
	
	
	protected void gatherData() {
		unpublishedWrites = new HashMap<String, Double>();
		
		// find the clients that wrote on providers without publishing the writes for each blob
		for (String blobID : blobs.getBlobIDs()) {
			
			blobQuery(blobID);

			while (db.next()) {
				// retrieve the current number of writes
				Double sizeOfUnpublishedWrites = unpublishedWrites.get(db.getString("client"));

				// initialize the number of writes if it is the first
				// time the selected client is put into the list
				if (sizeOfUnpublishedWrites == null) {
					sizeOfUnpublishedWrites = 0.0;
				}

				// add the number of writes for the current blob to
				// the current value
				sizeOfUnpublishedWrites += db.getDouble("no") * blobs.getBlobPageSize(blobID);
				// update the value for he current client
				unpublishedWrites.put(db.getString("client"), sizeOfUnpublishedWrites);
			}
		}
	}
	
	protected void blobQuery(String blobID){
		
		// search for the clients that wrote data to providers, 
		//but the watermark of their writes cannot be found in the received published writes
		db.executeQuery("SELECT writes.client_id AS client, count(writes.watermark) AS no " +
				"FROM (SELECT client_id, watermark FROM write_"+ blobID +" GROUP BY client_id,watermark) AS writes " +
				"WHERE NOT EXISTS " +
					"(" +
					"SELECT publishedWrites.watermark " +
					"FROM client_writes_" + blobID + " AS publishedWrites " +
					"WHERE publishedWrites.watermark = writes.watermark" +
 					") " +
 				"GROUP BY writes.client_id");
	}

	
	
}
