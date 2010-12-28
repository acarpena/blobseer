package processing.timers;

import java.util.TimerTask;

import processing.feedback.BlackList;


/**
 * @author acarpena
 *
 * The update task associated with the TimerUpdate.
 * It is called periodically and it updates the associated black list
 */
public class UpdateLocalListTask extends TimerTask {

	private BlackList list;
	
	
	public UpdateLocalListTask(BlackList list) {
		super();
		this.list = list;
	}


	@Override
	public void run() {
		
		if (list!= null)
			list.updateBlackList();

	}

}
