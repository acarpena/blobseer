/**
 * Class that manages the configuration file for security policies.
 * 
 * @author Cristina Basescu
 * @date 03.2010
 */
package util.conf;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.RandomAccessFile;
import java.util.StringTokenizer;

public class Conf {
	/* configuration file path */
	public static String confFile;
	/* current policy number */
	public static long lastPolicyNum = 30;
	public static String policyDir = "";
	public static String serDir = "";
	public static int windowSize = 50;
	public static String opConstants = "><=!+-\\*⊂";
	
	/**
	 * Reads the configuration file.
	 */
	public static void readConf() {
		try{
			System.out.println(confFile);
			// create file if it does not exist
			File f;
		    f = new File(confFile);
		    if(!f.exists()) {
		    	f.createNewFile();
		    	BufferedWriter out = new BufferedWriter(new FileWriter(confFile));
		    	out.write("policyDir="+policyDir+"\n");
		    	out.write("serDir="+serDir+"\n");
		    	out.write("windowSize="+windowSize+"\n");
		    	out.write("lastPolicyNum="+lastPolicyNum+"\n");
		    	out.close(); 
		    }
			// open file
		    FileInputStream fstream = new FileInputStream(confFile);
		    DataInputStream in = new DataInputStream(fstream);
		    BufferedReader br = new BufferedReader(new InputStreamReader(in));
		    String strLine;
		    //read line by line
		    while ((strLine = br.readLine()) != null)   {
		    	// skip comments
		    	if(!strLine.startsWith("//")) {
		    		StringTokenizer st = new StringTokenizer(strLine, "=");
		    		String next = st.nextToken(); 
		    		if(next.compareTo("lastPolicyNum") == 0)
		    			lastPolicyNum = Long.parseLong(st.nextToken());
		    		if(next.compareTo("policyDir") == 0)
		    			policyDir = st.nextToken();
		    		if(next.compareTo("serDir") == 0)
		    			serDir = st.nextToken();
		    		if(next.compareTo("windowSize") == 0)
		    			windowSize = Integer.parseInt(st.nextToken());
		    	}
		    }
		    // close the input stream
		    in.close();
		    }catch (Exception e){
		      System.err.println("Error: " + e.getMessage());
		      e.printStackTrace();
		    }
	}
	
	/**
	 * Increments policy number.
	 */
	public static void incPolicyNum() {
		lastPolicyNum++;
	}
	
	/**
	 * Writes the current lastPolicyNum in the configuration file.
	 */
	public static void writeConf() {
		try {
		     
            File file = new File(confFile);
            RandomAccessFile raf = new RandomAccessFile(file, "rw");

            String strLine;
            long crtPos = raf.getFilePointer();
            
            while ((strLine = raf.readLine()) != null) {
            	if(strLine.startsWith("lastPolicyNum"))
            		break;
            	crtPos = raf.getFilePointer();
            }
            raf.seek(crtPos);
            raf.writeBytes("lastPolicyNum="+lastPolicyNum+"\n");
            raf.close();
          
        } catch (IOException e) {
            System.out.println("IOException:");
            e.printStackTrace();
        }
	}
}
