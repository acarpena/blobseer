/*
      *writes to a file passed as argument
	*arguments: file name, no of clients, chunk size
      *generates chunks of 64MB of random data and writes each chunk to the file
*/

import java.io.IOException;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.Path;

public class WriteBigFile {
      static void usage ()
      {
	    System.out.println("Usage : WriteToFile <file> <no of clients> <chunk size>");
	    System.exit(1);
      }
      
      static void printAndExit(String str)
      {
	    System.err.println(str);
	    System.exit(1);
      }
      
      public static void main(String args[]) throws IOException
      {
	    if (args.length != 3)
		  usage();
	    
	    Configuration config = new Configuration();
	    Map<String, String> env = System.getenv();               
            String hadoop_home = env.get("HADOOP_HOME");                                                                                      
            if (hadoop_home != null)                                                                                                          
                  config.addResource(new Path(hadoop_home + "/conf/core-site.xml"));   	    
	    FileSystem fs = FileSystem.get(config);	  
	    Path f = new Path(args[0]);
	    FSDataOutputStream fos;
	    
	    int cl_no = Integer.parseInt(args[1]);
	    int chunk_size = 1 << (Integer.parseInt(args[2]));
	    byte buffer[] = new byte[chunk_size];
	    int total_size = cl_no * (1 << (Integer.parseInt(args[2]) - 20));	//total size in MB
	    	    
	    for (int i = 0; i < chunk_size; i++)
		  buffer[i] = (byte)(32 + i % 96);

	    try {
		 // Timer t = new Timer();
                  fos = fs.create(f);
		 // t.start();
		  for (int i = 0; i < cl_no; i++)
			fos.write(buffer, 0, chunk_size);
		 // System.out.println(t.getBandwidth(total_size));
		 System.out.println("wrote file"); 
		 fos.close();
	    } catch (IOException ioe) {
		  System.err.println("IOException during operation: " + ioe.toString());
		  System.exit(1);
	    }
      }
}

