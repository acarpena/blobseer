package feedback.userAccounting.maliciousClients;

import java.util.HashMap;

import mon.util.db.DatabaseAccess;
import feedback.userAccounting.blackList.BasicBlackList;


/**
 * @author acarpena
 *
 *  Class implementing a black list of clients that perform incorrect write operations
 */
public class IncorrectWrites extends BasicBlackList {

	HashMap<String, Double> incorrectWrites;	// the number of the published writes that have different sizes
												// than the corresponding writes on the providers (for each client)
	
	public IncorrectWrites(DatabaseAccess db) {
		super(db);
	}

	

	public void updateBlackList() {
		
		gatherData();
		
		synchronized (list) {
			list.clear();
			
			for (String el:incorrectWrites.keySet()) {
				list.add(new ClientScoreListElement(el, incorrectWrites.get(el).intValue()));
			}
		}
		logger.info(list.toString());
	}
	

	
	
	public void gatherData() {
	
		incorrectWrites = new HashMap<String, Double>();

		// find the clients that wrote on providers without publishing the writes for each blob
		for (String blobID : blobs.getBlobIDs()) {

			blobQuery(blobID);

			while (db.next()) {
				// retrieve the current number of writes
				Double sizeOfUnpublishedWrites = incorrectWrites.get(db.getString("client"));

				// initialize the number of writes if it is the first
				// time the selected client is put into the list
				if (sizeOfUnpublishedWrites == null) {
					sizeOfUnpublishedWrites = 0.0;
				}

				// add the number of writes for the current blob to
				// the current value
				sizeOfUnpublishedWrites += db.getDouble("no") * blobs.getBlobPageSize(blobID);
				// update the value for he current client
				incorrectWrites.put(db.getString("client"), sizeOfUnpublishedWrites);
			}
		}
		
	}
	
	protected void blobQuery(String blobID){
		
		// search for the clients that wrote on the providers a number of pages  
		// different than the published one
		/*db.executeQuery("SELECT writes.client_id AS client, count(writes.watermark) AS nowrites, " +
				"sum(publishedWrites.write_size) AS nopublished" +
				"FROM write_" + blobID + " AS writes, " +
					"client_writes_" + blobID + " AS publishedWrites, " +
				"WHERE writes.watermark=publishedWrites.watermark " +
				"AND NOT (nowrites=nopublished)" +
 				"GROUP BY writes.client_id"); */
		//db.executeQuery("SELECT writes.client_id AS client, nowrites, publishedWrites.sum_writes AS nopublished FROM (SELECT client_id, watermark,count(watermark) AS nowrites FROM write_"+blobID+" GROUP BY client_id,watermark) AS writes, (SELECT watermark,sum(write_size) AS sum_writes FROM client_writes_"+blobID+" GROUP BY watermark) AS publishedWrites WHERE (writes.watermark=publishedWrites.watermark) GROUP BY writes.client_id,nowrites,publishedWrites.sum_writes HAVING (nowrites <> (publishedWrites.sum_writes/"+blobs.getBlobPageSize(blobID)+") )");
		db.executeQuery("SELECT writes.client_id AS client, no, publishedWrites.sum_writes AS nopublished FROM (SELECT client_id, watermark,count(watermark) AS no FROM write_"+blobID+" GROUP BY client_id,watermark) AS writes, (SELECT watermark,sum(write_size) AS sum_writes FROM client_writes_"+blobID+" GROUP BY watermark) AS publishedWrites WHERE (writes.watermark=publishedWrites.watermark) GROUP BY writes.client_id,no,publishedWrites.sum_writes HAVING (no <> (publishedWrites.sum_writes/"+blobs.getBlobPageSize(blobID)+") )");
		// TODO de vazut ce e cu write-urile nealiniate
	}
	
}
