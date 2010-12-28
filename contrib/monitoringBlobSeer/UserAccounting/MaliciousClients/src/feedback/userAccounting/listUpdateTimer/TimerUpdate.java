package feedback.userAccounting.listUpdateTimer;

import java.util.Timer;

import feedback.userAccounting.blackList.PeriodicalUpdate;


/**
 * @author acarpena
 *
 * Class that implements an updater for a black list, based on a timer that periodically calls an update task
 */
public class TimerUpdate extends PeriodicalUpdate{

	private final int DEFAULT_DELAY  = 5000;
	private Timer timer;
	private int delay;
		
	public TimerUpdate() {
		super();
		timer = new Timer();
		delay = DEFAULT_DELAY;
	}
	
	public TimerUpdate(int msec) {
		super();
		timer = new Timer();
		delay = msec;
	}

	@Override
	public void startUpdating() {
		
		timer.schedule(new UpdateLocalListTask(list), delay, delay);
	}

	
}
