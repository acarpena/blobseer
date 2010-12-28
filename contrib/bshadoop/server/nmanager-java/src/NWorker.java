import java.nio.channels.SocketChannel;
import java.util.LinkedList;
import java.util.List;

/**
 * 
 * This is the worker thread that processes client requests and generates
 * the appropiate replies
 * 
 * The nio server adds new requests to be processed in a queue using the processData method.
 * The worker thread waits for requests to appear in the queue and treats them one-by-one,
 * sending the reply back to the nio server.
 * 
 * Requests are processed by the Namespace class. See its description for more details.
 */

public class NWorker implements Runnable {
	private List queue = new LinkedList();
	private Namespace namespace;
	
	public NWorker(String cfg) {
		namespace = new Namespace(cfg);
	}
	
	public void processData(NManager server, SocketChannel socket, byte[] data, int count) {
		synchronized(queue) {
			queue.add(new ServerDataEvent(server, socket, data, count));
			queue.notify();
		}
	}
	
	public void run() {
		ServerDataEvent dataEvent;
		while(true) {
			// Wait for data to become available
			synchronized(queue) {
				while(queue.isEmpty()) {
					try {
						queue.wait();
					} catch (InterruptedException e) {
					}
				}
				dataEvent = (ServerDataEvent) queue.remove(0);
			}
			// Return to sender
			dataEvent.server.send(dataEvent.socket, namespace.process(dataEvent.data, dataEvent.size));
		}
	}
}
