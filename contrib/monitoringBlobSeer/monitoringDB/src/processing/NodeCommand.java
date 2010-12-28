package processing;

import java.io.Serializable;

/**
 * encapsulates the two possible commands that can be sent to a processing node <br>
 * {@link #PROCESS_MONITOR_DATA} is sent by the MonALISA services when forwarding monitor data <br>
 * {@link #GATHER_CLIENT_INFO} is sent by the module computing the black list when gathering information 
 * from each node
 * @author Michi
 *
 */

public class NodeCommand implements Serializable
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 4754679902536508667L;
	private Serializable monitorData;
	private int command;
	public static final int PROCESS_MONITOR_DATA=1;
	public static final int GATHER_CLIENT_INFO=2;
	
	public NodeCommand(int command, Serializable data)
	{
		this.command = command;
		monitorData = data;
	}
	
	public int getCommand()
	{
		return command;
	}
	
	public Serializable getMonitorData()
	{
		return monitorData;
	}
}
