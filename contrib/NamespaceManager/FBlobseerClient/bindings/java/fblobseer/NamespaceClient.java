package fblobseer;

import java.io.IOException;
import java.util.Vector;
import java.nio.ByteBuffer;

public class NamespaceClient
{
    // the pointer the C++ NamespaceClient
    private long ns_handler;

    private final static native long ns_handler_init(String cfg);
    private final static native boolean ns_handler_finalize(long ns);

    private final static native int ns_handler_mkdir(long ns_handler, String path);
    private final static native ReturnValues ns_handler_exists(long ns_handler, String path);
    private final static native ReturnValues ns_handler_isfile(long ns_handler, String path);
    private final static native int ns_handler_rename(long ns_handler, String path, String newPath);
    private final static native boolean ns_handler_status(long ns_handler, String path, FileMetadata fileMetadata);
    private final static native int ns_handler_delete(long ns_handler, String path);
    private final static native boolean ns_handler_listdir(long ns_handler, String path, Vector<FileMetadata> fmList);
    
    private final static native long ns_handler_createFile(long ns_handler, String path, long pageSize, int replicaCount);
    private final static native long ns_handler_getFileHandler(long ns_handler, String path);
    private final static native void ns_handler_finalizeFileHandler(long fblob);
    
    static {
        try {
            System.loadLibrary("fbs_namespace-java");
        } catch (UnsatisfiedLinkError e) {
            e.printStackTrace();
            System.err.println("Unable to load libfbs_namespace-java native library");
            System.exit(1);
        }
    }

    // Create the NamespaceClient object
    public NamespaceClient(String cfg) throws IOException
    {
    	ns_handler = ns_handler_init(cfg);
    	if (ns_handler == 0) {
    		throw new IOException("Unable to initialize the namespace client using the configuration file " + cfg);
    	}
    }
    
    protected void finalize() throws Throwable {
    	try {
    		ns_handler_finalize(ns_handler);
    	} finally {
    		super.finalize();
    	}
    }

    // create a new directory
    public int mkDir(String path) {
    	return ns_handler_mkdir(ns_handler, path);
    }
    
    // check if a file/directory exists
    public ReturnValues exists(String path) {
    	ReturnValues rv =  ns_handler_exists(ns_handler, path);
        return rv;
    }
    
    // check if a path points to a file/directory
    public ReturnValues isFile(String path) {
    	ReturnValues rv =  ns_handler_isfile(ns_handler, path);
        return rv;
    }
    
    // rename a file/directory
    public int rename(String path, String newPath) {
    	return ns_handler_rename(ns_handler, path, newPath);
    }
    
    // get the status of a file
    public FileMetadata status(String path) {
    	FileMetadata fileMetadata = new FileMetadata();
    	
    	if (!ns_handler_status(ns_handler, path, fileMetadata))
    	{
    		return null;
    	}
    	return fileMetadata;
    }
    
    // delete a file/directory
    public int delete(String path) {
    	return ns_handler_delete(ns_handler, path);
    }
    
    // list a directory
    public Vector<FileMetadata> listDir(String path) {
    	Vector<FileMetadata> fmList = new Vector<FileMetadata>();
    	
    	if (!ns_handler_listdir(ns_handler, path, fmList))
    	{
    		return null;
    	}
    	return fmList;
    }
    
    // create a file
    public FileHandler createFile(String path, long pageSize, int replicaCount)  throws IOException {
    	return new BlobFileHandler(path, pageSize, replicaCount);
    }
    
    // open a file
    public FileHandler getFileHandler(String path)  throws IOException {
    	return new BlobFileHandler(path);
    }
    
    // class that implements a constructor for the FileHandler, 
    // because a FileHandler must be created/opened only through the 
    // create/getFileHandler methods of the namespace
    // to ensure the name of the file is associated with a blob
    private class BlobFileHandler extends FileHandler {
    	public BlobFileHandler(String path, long pageSize, int replicaCount) throws IOException {
    		fblob = NamespaceClient.ns_handler_createFile(NamespaceClient.this.ns_handler, path, pageSize, replicaCount);
    		if (fblob == 0) {
    			throw new IOException("Unable to initialize a new file with the path: " + path);
    		}
    	}
    	
    	public BlobFileHandler(String path)  throws IOException {
    		fblob = NamespaceClient.ns_handler_getFileHandler(NamespaceClient.this.ns_handler, path);
    		if (fblob == 0) {
    			throw new IOException("Unable to open a file with the path: " + path);
    		}

    	}
    	
        // Delete the blob when the Java GC decides to destroy this object
        protected void finalize() throws Throwable {
        	try {
        		ns_handler_finalizeFileHandler(fblob);
        	} finally {
        		super.finalize();
        	}
        }
    }


}
