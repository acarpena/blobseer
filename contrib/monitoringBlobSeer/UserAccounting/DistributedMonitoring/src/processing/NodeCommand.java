package processing;

import java.io.Serializable;
import data.MonitorData;

/**
 * encapsulates the two possible commands that can be sent to a processing node <br>
 * {@link #PROCESS_MONITOR_DATA} is sent by the MonALISA services when forwarding monitor data <br>
 * {@link #GATHER_CLIENT_INFO} is sent by the module computing the black list when gathering information 
 * from each node
 * @author Michi
 *
 */
@SuppressWarnings("serial")
public class NodeCommand implements Serializable
{
	private MonitorData monitorData;
	private int command;
	public static final int PROCESS_MONITOR_DATA=1;
	public static final int GATHER_CLIENT_INFO=2;
	
	public NodeCommand(int command, MonitorData data)
	{
		this.command = command;
		monitorData = data;
	}
	
	public int getCommand()
	{
		return command;
	}
	
	public MonitorData getMonitorData()
	{
		return monitorData;
	}
}