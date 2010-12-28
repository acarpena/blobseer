/*                                                                     
      *reads from a file passed as argument a chunk of 64 M                        
	* arguments: file, chunk size, client index
      *sequential reads of 4KB at a time from start to end             
*/                                                                     

import java.io.IOException;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;     
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.Path;             

public class ReadChunkFromFile {
      static void usage () 
      {                    
            System.out.println("Usage : ReadChunkFromFile <file> <chunk size> <client index> <retries> <logging_server_hostname> <logging_server_port>");
            System.exit(1);                                   
      }                                                       
                                                              
      static void printAndExit(String str)                    
      {                                                       
            System.err.println(str);                          
            System.exit(1);                                   
      }                                                       
                                                              
      public static void main(String args[]) throws IOException
      {                                                        
            if (args.length != 6)                              
                  usage();                                     
                                                               
            Configuration config = new Configuration();        
            Map<String, String> env = System.getenv();               

            String hadoop_home = env.get("HADOOP_HOME");
            if (hadoop_home != null)                    
                  config.addResource(new Path(hadoop_home + "/conf/core-site.xml"));                                         
                                                                       
            FileSystem fs = FileSystem.get(config);                    
                                                                       
            Path f = new Path(args[0]);                                
            FSDataInputStream fis;                                     
            byte buffer[] = new byte[4096];                            
	    long chunk_size = Long.parseLong(args[1]);
	    long client_index = Long.parseLong(args[2]);
	    long rounds = 1 << (chunk_size - 12);
              
            if (!fs.exists(f))                                         
                  printAndExit("File " + f + " not found");            

            if (!fs.isFile(f))
                  printAndExit(f + " is not a file");

	    int retry = Integer.parseInt(args[3]);

	    long failed_chunk = -1;
            LogClient log = null;
	    
            try {
                  log = new LogClient(args[4], Integer.parseInt(args[5]));
		  fis = fs.open(f);
		  int failed = 0;
		  fis.seek((client_index - 1) * (1 << chunk_size));
                  log.logMessage("# starts reading");	  
                  for (long i = 0; i < rounds; i++) {
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
            } catch (IOException ioe) {
                  System.err.println("IOException during operation: " + ioe.toString());
                  System.exit(1);
            }
      }
}

