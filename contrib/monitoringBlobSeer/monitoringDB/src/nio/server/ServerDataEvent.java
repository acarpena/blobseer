package nio.server;

import java.nio.channels.SocketChannel;


public class ServerDataEvent {
        public NioServer server;
        public SocketChannel socket;
        public byte[] data;
        public int size;

        public ServerDataEvent(NioServer server, SocketChannel socket, byte[] data, int size) {
                this.server = server;
                this.socket = socket;
                this.size = size;
                this.data = new byte[this.size];
                System.arraycopy(data, 0, this.data, 0, this.size);
        }
}