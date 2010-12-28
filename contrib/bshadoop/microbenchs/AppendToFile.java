/*
  write X MB to hadoop fs
*/

import java.io.IOException;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.Path;

public class AppendToFile {
    static void usage () {
	System.out.println("Usage : AppendToFile <file> <size> <retry> <logging_server_hostname> <logging_server_port>");
	System.exit(1);
    }
      
    static void printAndExit(String str) {
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
	int magnitude = 30;
	try {
	    magnitude = Integer.parseInt(args[1]);
	} catch(Exception e) {
	    System.out.println("Second argument invalid. Assuming 30 (1GB)");
	}

	int buffer_size = 1 << 26;		//64M
	byte buffer[] = new byte[buffer_size];
	long chunks = (((long)1) << magnitude) / buffer_size;
	    
	for (int i = 0; i < buffer_size; i++)
	    buffer[i] = (byte)(32 + i % 96);

	int retry = Integer.parseInt(args[2]);
	long failed_chunk = -1;
        LogClient log = null;
        try {
            log = new LogClient(args[3], Integer.parseInt(args[4]));

	    FSDataOutputStream fos = fs.append(f);
	    int failed = 0;
            log.logMessage("# starts writing");
	    for (long i = 0; i < chunks; i++) {
		int retry_chunk = 0;
                while (retry_chunk < retry) {
                     failed = 0;
                     try {
                        // log.logMessage("# starts writing chunk " + i);
                        fos.write(buffer, 0, buffer_size);
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
	} catch (IOException ioe) {
	    System.err.println("IOException during operation: " + ioe.toString());
	    System.exit(1);
	}
    }
}

