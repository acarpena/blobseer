import java.io.IOException;
//import java.io.FileOutputStream;
//import java.io.File;
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
// import java.util.Calendar;                                                                                                                                    
// import java.text.SimpleDateFormat; 

public class LogServer implements Runnable
{
      // The host:port combination to listen on
      private InetAddress hostAddress;
      private int port;
      
      // The channel on which we'll accept connections
      private ServerSocketChannel serverChannel;
      
      // The selector we'll be monitoring
      private Selector selector;
      
      // The buffer into which we'll read data when it's available
      private ByteBuffer readBuffer = ByteBuffer.allocate(1024);
      
      // A list of PendingChange instances
      private List pendingChanges = new LinkedList();
      
//       private String DATE_FORMAT_NOW = "yyyy-MM-dd HH:mm:ss:SS";
      
//       private FileOutputStream logFile;
      
/*      public String now() 
      {
	    Calendar cal = Calendar.getInstance();
	    SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT_NOW);
	    return sdf.format(cal.getTime());                            
      }                                       
*/      
      public LogServer(InetAddress hostAddress, int port) throws Exception
      {
	    this.hostAddress = hostAddress;
	    this.port = port;
	    this.selector = this.initSelector();
// 	    File f = new File(fn);
// 	    logFile = new FileOutputStream(f);
      }
      
      private void logMessage(byte[] b, int len)
      {
	    try{
		  System.out.print("[" + System.nanoTime() + "]");
		  System.out.write(b, 0, len);
	    }
	    catch (Exception e) {
		  e.printStackTrace();
	    }
      }
      
      public void run()
      {
	    while (true) {
		  try {
			// Process any pending changes
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
			      } /*else if (key.isWritable()) {
				    this.write(key);
			      }*/
			}
		  } catch (Exception e) {
			e.printStackTrace();
		  }
	    }
      }
      
      private void accept(SelectionKey key) throws IOException
      {
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
      
      private void read(SelectionKey key) throws IOException
      {
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
	    //logMessage(new String(readBuffer.array()));
	    logMessage(readBuffer.array(), numRead);
      }
      
      private Selector initSelector() throws IOException
      {
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
      
      public static void main(String[] args)
      {
	    if (args.length != 1) {
		  System.out.println("Usage: LogServer <listening_port>");
		  return;
	    }
	    try {
		  new Thread(new LogServer(null, Integer.parseInt(args[0]))).start();
	    } catch (Exception e) {
		  e.printStackTrace();
	    }
	    System.out.println("listening on " + args[0]);
	}
}
