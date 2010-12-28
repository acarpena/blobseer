import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.channels.spi.SelectorProvider;
import java.util.*;
import java.util.Calendar;                                                                                                                                    
import java.text.SimpleDateFormat; 

/**
 * This is the nio server side implementation.
 * It handles multiple connections using a dedicated selector thread. 
 *  
 * The nio server reads a request from each socket opened by a client and enqueues
 * the requests in a read queue that a worker thread will process. The worker thread
 * generates and passes the replies to the nio server. The nio server writes the
 * replies back to the corresponding sockets, then tries to read a new request from
 * each socket. The client-side must close the socket once it has no more requests.
 */

public class NManager implements Runnable {
	// The host:port combination to listen on
	private InetAddress hostAddress;
	private int port;

	// The channel on which we'll accept connections
	private ServerSocketChannel serverChannel;

	// The selector we'll be monitoring
	private Selector selector;

	// The buffer into which we'll read data when it's available
	private ByteBuffer readBuffer = ByteBuffer.allocate(8192);

	private NWorker worker;

	// A list of PendingChange instances
	private List pendingChanges = new LinkedList();

	// Maps a SocketChannel to a list of ByteBuffer instances
	private Map pendingData = new HashMap();
	
	private int con;

        private String DATE_FORMAT_NOW = "yyyy-MM-dd HH:mm:ss:SS";

        public String now() 
        {                   
            Calendar cal = Calendar.getInstance();
            SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT_NOW);
            return sdf.format(cal.getTime());                            
        }                                       

	public NManager(InetAddress hostAddress, int port, NWorker worker) throws IOException {
		this.hostAddress = hostAddress;
		this.port = port;
		this.selector = this.initSelector();
		this.worker = worker;
		this.con = 0;
	}

	public void send(SocketChannel socket, byte[] data) {
		synchronized (this.pendingChanges) {
			// Indicate we want the interest ops set changed
			this.pendingChanges.add(new ChangeRequest(socket, ChangeRequest.CHANGEOPS, SelectionKey.OP_WRITE));

			// And queue the data we want written
			synchronized (this.pendingData) {
				List queue = (List) this.pendingData.get(socket);
				if (queue == null) {
					queue = new ArrayList();
					this.pendingData.put(socket, queue);
				}
				queue.add(ByteBuffer.wrap(data));
			}
		}

		// Finally, wake up our selecting thread so it can make the required changes
		this.selector.wakeup();
	}

	public void run() {
		while (true) {
			try {
				// Process any pending changes
				synchronized (this.pendingChanges) {
					Iterator changes = this.pendingChanges.iterator();
					while (changes.hasNext()) {
						ChangeRequest change = (ChangeRequest) changes.next();
						switch (change.type) {
						case ChangeRequest.CHANGEOPS:
							SelectionKey key = change.socket.keyFor(this.selector);
							key.interestOps(change.ops);
						}
					}
					this.pendingChanges.clear();
				}
				// Wait for an event one of the registered channels
				this.selector.select();

				// Iterate over the set of keys for which events are available
				Iterator selectedKeys = this.selector.selectedKeys().iterator();
				while (selectedKeys.hasNext()) {
					SelectionKey key = (SelectionKey) selectedKeys.next();
					selectedKeys.remove();

					if (!key.isValid()) {
						continue;
					}

					// Check what event is available and deal with it
					if (key.isAcceptable()) {
						this.accept(key);
					} else if (key.isReadable()) {
						this.read(key);
					} else if (key.isWritable()) {
						this.write(key);
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	private void accept(SelectionKey key) throws IOException {
		// For an accept to be pending the channel must be a server socket channel.
		ServerSocketChannel serverSocketChannel = (ServerSocketChannel) key.channel();

		// Accept the connection and make it non-blocking
		SocketChannel socketChannel = serverSocketChannel.accept();
		Socket socket = socketChannel.socket();
		socketChannel.configureBlocking(false);

		// Register the new SocketChannel with our Selector, indicating
		// we'd like to be notified when there's data waiting to be read
		socketChannel.register(this.selector, SelectionKey.OP_READ);
	}

	private void read(SelectionKey key) throws IOException {
		SocketChannel socketChannel = (SocketChannel) key.channel();

		// Clear out our read buffer so it's ready for new data
		this.readBuffer.clear();

		// Attempt to read off the channel
		int numRead;
		try {
			numRead = socketChannel.read(this.readBuffer);
		} catch (IOException e) {
			// The remote forcibly closed the connection, cancel
			// the selection key and close the channel.
			key.cancel();
			socketChannel.close();
			return;
		}

		if (numRead == -1) {
			// Remote entity shut the socket down cleanly. Do the
			// same from our end and cancel the channel.
			key.channel().close();
			key.cancel();
			return;
		}

		// Hand the data off to our worker thread
		if (numRead > 0)
			this.worker.processData(this, socketChannel, this.readBuffer.array(), numRead);
	}

	private void write(SelectionKey key) throws IOException {
		SocketChannel socketChannel = (SocketChannel) key.channel();

		synchronized (this.pendingData) {
			List queue = (List) this.pendingData.get(socketChannel);

			// Write until there's not more data ...
			while (!queue.isEmpty()) {
				ByteBuffer buf = (ByteBuffer) queue.get(0);
				socketChannel.write(buf);
				if (buf.remaining() > 0) {
					// ... or the socket's buffer fills up
					break;
				}
				queue.remove(0);
			}

			if (queue.isEmpty()) {
				// We wrote away all data, so we're no longer interested
				// in writing on this socket. Switch back to waiting for
				// data.
				key.interestOps(SelectionKey.OP_READ);
			}
		}
	}

	private Selector initSelector() throws IOException {
		// Create a new selector
		Selector socketSelector = SelectorProvider.provider().openSelector();

		// Create a new non-blocking server socket channel
		this.serverChannel = ServerSocketChannel.open();
		serverChannel.configureBlocking(false);

		// Bind the server socket to the specified address and port
		InetSocketAddress isa = new InetSocketAddress(this.hostAddress, this.port);
		serverChannel.socket().bind(isa);

		// Register the server socket channel, indicating an interest in 
		// accepting new connections
		serverChannel.register(socketSelector, SelectionKey.OP_ACCEPT);
		return socketSelector;
	}

	public static void main(String[] args) {
		if (args.length != 2) {
			System.out.println("Usage: NManager <listening_port> <blobseer_conf_file>");
			return;
		}
		try {
			NWorker worker = new NWorker(args[1]);
			new Thread(worker).start();
			new Thread(new NManager(null, Integer.parseInt(args[0]), worker)).start();
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("listening on " + args[0]);
	}
}
