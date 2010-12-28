package nio.server;

import java.nio.channels.SocketChannel;

public interface ServerWorker 
{
	public void processData(NioServer server, SocketChannel socket, byte[] data, int count);
}
