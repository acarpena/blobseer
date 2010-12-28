package feedback.userAccounting.blackList;

import java.util.Vector;

import mon.util.db.DatabaseAccess;

/**
 * @author acarpena
 *
 * Generic class that implements a blacklist that 
 *  combines data obtained from multiple lists
 */
public abstract class MultipleBlackLists extends BasicBlackList {

	// the array of lists that will have their output combined
	protected Vector<BasicBlackList> blackLists;
	

	public MultipleBlackLists(Vector<DatabaseAccess> dbs) {
		super();
		blackLists = new Vector<BasicBlackList>();
		initializeLists(dbs);
	}

	
	// retrieve the value of each list and then update the combined list
	public void updateBlackList() {

		Vector<Vector<BlackListElement>> results = new Vector<Vector<BlackListElement>>();
		
		for (BasicBlackList l:blackLists) {
			results.add(l.getList());
		}

		updateBigList(results);
		logger.info(list.toString());
	}

	
	// update all the lists with the same type of PeriodicalUpdate
	public void setUpdateType(PeriodicalUpdate p) {
		
		super.setUpdateType(p);
		for (BasicBlackList l:blackLists) {
			l.setUpdateType(p);
		}
	}
	
	
	// update each list in a different way
	public void setUpdateType(Vector<PeriodicalUpdate> p) {
		
		super.setUpdateType(p.get(0));
		for (int i=1; i<blackLists.size(); i++) {
			BasicBlackList l = blackLists.get(i);
			PeriodicalUpdate pu = p.get(i);
			l.setUpdateType(pu);
		}
	}

	// update the combined list 
	public abstract void updateBigList(Vector<Vector<BlackListElement>> results);
	public abstract void initializeLists(Vector<DatabaseAccess> dbs);

}
