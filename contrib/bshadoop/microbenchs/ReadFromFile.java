/*
      *reads from a file passed as argument
	* arguments: file name, file size
      *sequential reads of 4KB at a time from start to end
*/

import java.io.IOException;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.Path;

public class ReadFromFile {
      static void usage ()
      {
	    System.out.println("Usage : ReadFromFile <file> <size> <retries> <logging_server_hostname> <logging_server_port>");
	    System.exit(1);
      }
      
      static void printAndExit(String str)
      {
	    System.err.println(str);
	    System.exit(1);
      }
      
      public static void main(String args[]) throws IOException
      {
	    if (args.length != 5)
		 usage();
	    
	    Configuration config = new Configuration();
	    Map<String, String> env = System.getenv();               

	    String hadoop_home = env.get("HADOOP_HOME");
            if (hadoop_home != null)                    
            	  config.addResource(new Path(hadoop_home + "/conf/core-site.xml"));        
	    
	    FileSystem fs = FileSystem.get(config);
	  
	    Path f = new Path(args[0]);
	    FSDataInputStream fis;
	
	    int size = 1 << 26;                 //64M                             
            byte buffer[] = new byte[size];
	    
	    int magnitude = 30;        
	    try {                      
        	 magnitude = Integer.parseInt(args[1]);
	    } catch(Exception e) {                    
         	 System.out.println("Second argument invalid. Assuming 30 (1GB)");                                                                 
	    }   

	    if (!fs.exists(f))
		  printAndExit("File " + f + " not found");

	    if (!fs.isFile(f))
		  printAndExit(f + " is not a file");
	   
	    int retry = Integer.parseInt(args[2]);

	    long failed_chunk = -1;
            LogClient log = null;
	    long chunks = (((long)1) << magnitude) / size; 
 
            try {
                  log = new LogClient(args[3], Integer.parseInt(args[4]));
                  fis = fs.open(f);
                  int failed = 0;
                  log.logMessage("# starts reading");	  
                  for (long i = 0; i < chunks; i++) {
                        int retry_chunk = 0;
                        while (retry_chunk < retry) {
                              failed = 0;
                              try {
                                    fis.read(buffer);
                              }
                              catch (IOException e) {
                                    failed = 1;
                                    e.printStackTrace();
                                    retry_chunk ++;
                              }
                              if (failed == 0)
                                    break;
                              log.logMessage("# failed reading, retries left " + (retry - retry_chunk));
                        }
                  }
                  if (failed == 0)
                          log.logMessage("# finished reading");
                  else
                          log.logMessage("# failed reading");
                  fis.close();
            }
            catch (IOException ioe) {
                  ioe.printStackTrace();
                  System.exit(1);
            }

/*
	    LogClient log = null;
	    try {
		  log = new LogClient(args[2], Integer.parseInt(args[3]));
		  fis = fs.open(f);
		//  Timer t = new Timer();
		//  t.start();
		  log.logMessage("# starts reading");
		  while (fis.read(buffer) > 0);
		  log.logMessage("# finished reading");
	  	 // System.out.println(t.getBandwidth(1 << (magnitude - 20)));
		  fis.close();
	    } catch (IOException ioe) {
		  System.err.println("IOException during operation: " + ioe.toString());
		  log.logMessage("# failed reading");
		  System.exit(1);
	    }*/
      }
}

