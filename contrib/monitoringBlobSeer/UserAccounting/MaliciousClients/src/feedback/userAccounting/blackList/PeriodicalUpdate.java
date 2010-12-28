package feedback.userAccounting.blackList;


/**
 * @author acarpena
 *
 * Generic class that implements a way to  periodically update a black list
 */
public abstract class PeriodicalUpdate {
	
	protected BasicBlackList list;

	
	public PeriodicalUpdate() {

		this.list = null;
	}
	
	
	public void setList(BasicBlackList l) {
		
		list = l;
	}

	// start the periodical update
	public abstract void startUpdating();
	
}
