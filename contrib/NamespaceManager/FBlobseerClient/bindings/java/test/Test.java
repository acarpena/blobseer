import fblobseer.FileHandler;
import fblobseer.NamespaceClient;
import fblobseer.FileMetadata;

import java.io.IOException;
import java.util.Vector;
import java.nio.ByteBuffer;

class Test {	
	private static final int BUFFER_SIZE = 1024;
	private static final int PAGE_SIZE = 8;

	
	public static void printList(Vector<FileMetadata> vec)
	{
		if (vec == null)
		{
			System.out.println("Error in listdir");
			return;
		}
		System.out.println("Listdir returned: ");
		for (int i=0;i<vec.size(); i++)
		{
			FileMetadata fm = vec.elementAt(i);
			System.out.println(fm.getPath() + "\t" + fm.getType());
		}
	}
	
	
	public static void main(String[] args) {	    
		if(args.length != 1) {
			System.out.println("Usage : java Test <configfile>");
			return;
		}

		NamespaceClient ns;
		FileHandler fileHandler;
		int res;

		ByteBuffer buffer = ByteBuffer.allocateDirect(2 * BUFFER_SIZE);
		for (int i = 0; i < 2 * BUFFER_SIZE; i++)
			buffer.put(i, (byte)0);

		// create the namespace client
		try {
			ns = new NamespaceClient(args[0]);
		} catch(IOException e){
			System.err.println("IOException catched.");
			return;
		}


		res = ns.mkDir("/dir1");
		System.out.println("Mkdir result: " + res);

		res = ns.mkDir("/dir2");
		System.out.println("Mkdir result: " + res);

		res = ns.mkDir("/dir1/dir3");
		System.out.println("Mkdir result: " + res);

		res = ns.mkDir("/dir1/dir4");
		System.out.println("Mkdir result: " + res);

		Vector<FileMetadata> fmList = ns.listDir("/dir1");
		printList(fmList);

		// rename
		res = ns.rename("/dir1/dir4", "/dir1/newDir");
		System.out.println("Rename result: " + res);
		
		Vector<FileMetadata> fmList1 = ns.listDir("/dir1");
		printList(fmList1);
		
		
		// delete
		res = ns.delete("/dir1/newDir");
		System.out.println("Delete result: " + res);
		
		Vector<FileMetadata> fmList2 = ns.listDir("/dir1");
		printList(fmList2);
		
		
		// create a file (path, page_size, replication_degree)
		try {
			fileHandler = ns.createFile("/dir1/file1", PAGE_SIZE, 1);
		} catch(IOException e){
			System.err.println("IOException catched.");
			return;
		}

		// write into the file
		for(int i=0; i<BUFFER_SIZE; i++) 
			buffer.put(i, (byte)i);
		// the write operation (offset, write_size, buffer)
		boolean ok;
		ok = fileHandler.write(0, BUFFER_SIZE, buffer);
		System.out.println("Write result: " + ok);

		// read from the file
		ByteBuffer result = ByteBuffer.allocateDirect(BUFFER_SIZE);
		for(int i = 0; i < BUFFER_SIZE; i++) 
			result.put(i, (byte)0);
		// the read operation (offset, read_size, buffer)
		fileHandler.read(0, BUFFER_SIZE, result);

		// compare the result with what we have written
		boolean c = true;
		for(int i = 0; i < BUFFER_SIZE; i++) 
			c = c & (result.get(i) == (byte)(i % 256));
		System.out.println("Read data is identical with the written data:" + c);

		// get the size of the file
		long size = fileHandler.getSize();
		System.out.println("File size: " + size);
		
		
/*		// open a file (path)
		try {
			fileHandler = ns.getFileHandler("/dir1/file1");
		} catch(IOException e){
			System.err.println("IOException catched.");
			return;
		}

		// write into the file
		for(int i=0; i<BUFFER_SIZE; i++) 
			buffer.put(i, (byte)i);
		// the write operation (offset, write_size, buffer)
		ok = fileHandler.write(0, BUFFER_SIZE, buffer);
		System.out.println("Write result: " + ok);

		// read from the file
		result = ByteBuffer.allocateDirect(BUFFER_SIZE);
		for(int i = 0; i < BUFFER_SIZE; i++) 
			result.put(i, (byte)0);
		// the read operation (offset, read_size, buffer)
		fileHandler.read(0, BUFFER_SIZE, result);

		// compare the result with what we have written
		c = true;
		for(int i = 0; i < BUFFER_SIZE; i++) 
			c = c & (result.get(i) == (byte)(i % 256));
		System.out.println("Read data is identical with the written data:" + c);

		
		// get the status of a file
		FileMetadata file = ns.status("/dir1/file1");
		if (file == null)
		{
			System.out.println("Error when getting the status for a file");
		}	
		System.out.println(file.getPath() + "\t" + file.getType());
		
		// list the directory that contains a file
		Vector<FileMetadata> fmList3 = ns.listDir("/dir1");
		printList(fmList3);
 */
	}
}
