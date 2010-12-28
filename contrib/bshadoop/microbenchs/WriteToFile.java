/*
      *writes to a file passed as argument
	* arguments: file, size
      *generates chunks of 64MB of random data and writes each chunk to the file
*/

import java.io.IOException;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.Path;

public class WriteToFile {
      static void usage ()
      {
	    System.out.println("Usage : WriteToFile <file> <size> <retry> <logging_server_hostname> <logging_server_port>");
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
            FSDataOutputStream fos;

            int magnitude = 30;
            try {
                 magnitude = Integer.parseInt(args[1]);
            } catch(Exception e) {
                 System.out.println("Second argument invalid. Assuming 30 (1GB)");
            }
            int retry = Integer.parseInt(args[2]);
            int size = 1 << 26;                 //64M
            byte buffer[] = new byte[size];
            long chunks = (((long)1) << magnitude) / size;

            for (int i = 0; i < size; i++)
                  buffer[i] = (byte)(32 + i % 96);

            long failed_chunk = -1;
            LogClient log = null;
            try {
                  log = new LogClient(args[3], Integer.parseInt(args[4]));
                  fos = fs.create(f);
		  int failed = 0;
		  log.logMessage("# starts writing");
		  for (long i = 0; i < chunks; i++) {
			int retry_chunk = 0;
			while (retry_chunk < retry) {
			      failed = 0;
			      try {
				   // log.logMessage("# starts writing chunk " + i);
				    fos.write(buffer, 0, size);
				   // log.logMessage("# finished writing chunk " + i);
			      }
			      catch (IOException e) {
				    failed = 1;
				    e.printStackTrace();
				    retry_chunk ++;
			      }
			      if (failed == 0)
				    break;
			      log.logMessage("# failed writing, retries left " + (retry - retry_chunk));
			}
		  }
		  if (failed == 0)
			  log.logMessage("# finished writing");
		  else
			  log.logMessage("# failed writing");
		  fos.close();
	    }
	    catch (IOException ioe) {
		  ioe.printStackTrace();
		  System.exit(1);
	    }
      }
}



