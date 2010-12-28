package feedback.userAccounting.listUpdateTimer;

import java.util.TimerTask;

import feedback.userAccounting.blackList.BasicBlackList;


/**
 * @author acarpena
 *
 * The update task associated with the TimerUpdate.
 * It is called periodically and it updates the associated black list
 */
public class UpdateLocalListTask extends TimerTask {

	private BasicBlackList list;
	
	
	public UpdateLocalListTask(BasicBlackList list) {
		super();
		this.list = list;
	}


	@Override
	public void run() {
		
		if (list!= null)
			list.updateBlackList();

	}

}
