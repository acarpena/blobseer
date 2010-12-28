package feedback.userAccounting.maliciousClients;

import java.util.HashMap;
import java.util.Vector;

import feedback.userAccounting.blackList.BlackListElement;
import feedback.userAccounting.blackList.MultipleBlackLists;
import mon.util.db.DatabaseAccess;


/**
 * @author acarpena
 *
 * Class that generates a malicious clients' list, combining data about the clients that
 *  that performed one of the three types of illegal write operations: incorrect writes, 
 *  writes on the data providers without the notification of the version manager and
 *  publication of writes to the version manager without accomplishing the actual data writes.
 */
public class MaliciousClients extends MultipleBlackLists {
	

	public MaliciousClients(Vector<DatabaseAccess> dbs) {
		super(dbs);
	}


	// the black list combines 3 types of lists
	@Override
	public void initializeLists(Vector<DatabaseAccess> dbs) {
		blackLists.add(new IncorrectWrites(dbs.get(0)));
		blackLists.add(new PublishNoWrite(dbs.get(1)));
		blackLists.add(new WriteNoPublish(dbs.get(2)));
		
	}


	// the black list is updated by taking the clients from all the lists and adding their scores obtained in each of them
	@Override
	public void updateBigList(Vector<Vector<BlackListElement>> results) {
		
		HashMap<String, Double> userList = new HashMap<String, Double>();
		
		for (Vector<BlackListElement> l:results) {
			for (BlackListElement el:l) {
				ClientScoreListElement clElem = (ClientScoreListElement)el;
				String cID = clElem.getClientID();
				
				Double currentScore = userList.get(cID);
				if (currentScore != null){ //existing client
					userList.put(cID, clElem.score + currentScore);
				}
				else { // first occurrence of client cID
					userList.put(cID, clElem.score + 0.0);
				}
					
			}
		}
		
		// put all the clients in the black list
		synchronized (list) {
			list.clear();
			
			for (String cl:userList.keySet()) {
				list.add(new ClientScoreListElement(cl, userList.get(cl).intValue()));
			}
		}
		
	}
}
