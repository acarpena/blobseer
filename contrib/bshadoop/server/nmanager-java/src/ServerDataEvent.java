import java.nio.channels.SocketChannel;

class ServerDataEvent {
	public NManager server;
	public SocketChannel socket;
	public byte[] data;
	public int size;
	
	public ServerDataEvent(NManager server, SocketChannel socket, byte[] data, int size) {
		this.server = server;
		this.socket = socket;
		this.size = size;
		this.data = new byte[this.size];
		System.arraycopy(data, 0, this.data, 0, this.size);
	}
}
