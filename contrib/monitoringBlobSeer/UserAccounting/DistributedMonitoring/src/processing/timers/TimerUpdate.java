package processing.timers;

import java.util.Timer;

import processing.feedback.BlackList;

/**
 * @author acarpena
 *
 * Class that implements an updater for a black list, based on a timer that periodically calls an update task
 */
public class TimerUpdate
{
	private final int DEFAULT_DELAY  = 5;
	private Timer timer;
	private int delay;
	protected BlackList list;
	
	public void setList(BlackList l) 
	{
		list = l;
	}

	public TimerUpdate() 
	{
		this.list = null;
		timer = new Timer();
		delay = DEFAULT_DELAY;
	}
	
	public TimerUpdate(int sec) {
		this.list = null;
		timer = new Timer();
		delay = sec;
	}

	public void startUpdating() {
		
		timer.schedule(new UpdateLocalListTask(list), delay*1000, delay*1000);
	}
	
}
