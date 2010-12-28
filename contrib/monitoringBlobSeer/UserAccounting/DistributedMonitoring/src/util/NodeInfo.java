package util;

/**
 * 
 * class that encapsulates the hostname and port on which a processing node listens for connections
 *
 */
public class NodeInfo
{
	String nodeName;
	int nodePort;
	
	public NodeInfo(String s, int port)
	{
		nodeName = s;
		nodePort = port;
	}
	
	public int getPort()
	{
		return nodePort;
	}
	
	public String getNodeName()
	{
		return nodeName;
	}
}
