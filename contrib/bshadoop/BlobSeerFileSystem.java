package org.apache.hadoop.fs.bsfs;
/* =================================
 * BlobSeer FileSystem for Hadoop
 * this file is part of the work conducted in KerData
 * team (IRISA - Rennes, Fr)
 * ================================= */
// java import
import java.io.*;
import java.net.*;
import java.util.*;

// hadoop import
import org.apache.hadoop.util.Progressable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.permission.FsPermission;
import org.apache.hadoop.fs.bsfs.*;
import org.apache.hadoop.fs.bsfs.client.*;
import org.apache.hadoop.fs.BlockLocation;

// BlobSeer import
import blobseer.ObjectHandler;
import blobseer.PageLocation;

/*  =================================
 *  Class BlobSeerFileSystem
 *  extends the FileSystem class in order to use Blobseer
 *  as versioning storage-space provider
 *  ================================= */
public class BlobSeerFileSystem extends FileSystem {
    static{
	Configuration.addDefaultResource("hdfs-default.xml");
	Configuration.addDefaultResource("hdfs-site.xml");
    }

    private URI uri;
    private Path workingDir = new Path("/"); // working directory (root)
    private BSFSClient nameNodeClient;  // client to interact with the name node server
    private String blobseerConfig; // blobseer configuration filename
    private long defaultBlockSize;
    private long defaultCacheSize;
    private ObjectHandler handler;
	
    // constructor
    public BlobSeerFileSystem() {
	// do nothing
    }
	
    // create a new file
    public FSDataOutputStream create(Path f, boolean overwrite, int bufferSize, short replication, long blockSize, Progressable progress) throws IOException {
	if(nameNodeClient.exists(makeAbsolute(f))) {
	    if(overwrite)
		nameNodeClient.delete(makeAbsolute(f));
	    else
		throw new IOException("Request to create existing file " + f);
	}
	
	if(nameNodeClient.create(makeAbsolute(f),(int)replication,defaultBlockSize)){
	    int blobid = nameNodeClient.blobid(makeAbsolute(f));
	    if(blobid == -1)
		return null;
	    else{
		BSFSCachedOutputStream outputStream = new BSFSCachedOutputStream(blobseerConfig,blobid,(int)defaultCacheSize);
		    outputStream.setNameNodeClient(nameNodeClient,makeAbsolute(f));
		return new FSDataOutputStream(outputStream);
	    }
	} else
	    throw new IOException("Could not create file " + f);
    }
	
    // check if a path exists
    public boolean exists(Path f) throws IOException {
	return nameNodeClient.exists(makeAbsolute(f));
    }
	
    // get the status of a file
    public FileStatus getFileStatus(Path f)  throws IOException {
	if(!nameNodeClient.exists(makeAbsolute(f))) throw new FileNotFoundException("File " + f + " does not exist.");
	return nameNodeClient.status(makeAbsolute(f));
    }
	
    // get uri
    public URI getUri() {
	return uri;
    }
	
    // get the working directory
    public Path getWorkingDirectory() {
	return workingDir;
    }
	
    // initialize the file system
    public void initialize(URI name, Configuration conf) throws IOException {
	super.initialize(name, conf);
	this.uri = URI.create(name.getScheme() + "://" + name.getAuthority());
	try {
	    this.nameNodeClient = new BSFSClient(uri.getHost(), uri.getPort());
	} catch (Exception e) {
	    e.printStackTrace();
	    System.out.println("Unable to initialize BSFS");
	    System.exit(-1);
	}
	this.defaultBlockSize = conf.getLong("fs.bsfs.page.size", -1);
	this.defaultCacheSize = conf.getLong("fs.bsfs.cache.size", -1)*this.defaultBlockSize;
	this.workingDir = new Path("/");
	this.blobseerConfig = conf.get("fs.bsfs.blobseerConfig","");
	this.handler = new ObjectHandler(blobseerConfig);
	setConf(conf);
    }
	
    // get the status of a file or a list of file/dir contained in a directory
    public FileStatus[] listStatus(Path f) throws IOException {
	return nameNodeClient.listdir(makeAbsolute(f));
    }
	
    // mkdirs create a new directory
    public boolean mkdirs(Path f, FsPermission permission) throws IOException {
	return nameNodeClient.mkdirs(makeAbsolute(f));
    }
	
    // delete a file or directory
    public boolean delete(Path f) throws IOException {
	return nameNodeClient.delete(makeAbsolute(f));
    }
	
    // delete a file or directory, recursively if needed
    public boolean delete(Path f, boolean recursively) throws IOException {
	if(nameNodeClient.isfile(makeAbsolute(f)))
	    return delete(makeAbsolute(f));
	else if(recursively)
	    return delete(makeAbsolute(f));
	else
	    throw new IOException("Tried to delete a directory with recursive set to false");
    }
	
    // open a file in read mode
    public FSDataInputStream open(Path f, int bufferSize) throws IOException {
	int blobid = nameNodeClient.blobid(makeAbsolute(f));
	FileStatus status = nameNodeClient.status(makeAbsolute(f));
	if(blobid == -1)
	    throw new IOException("Could not find blob associated to " + f);
	else{
	    BSFSCachedInputStream inputStream = new BSFSCachedInputStream(blobseerConfig,blobid, (int)this.defaultCacheSize, nameNodeClient, makeAbsolute(f));
	    return new FSDataInputStream(inputStream);
	}
    }
	
    // rename a file
    @Override
    public boolean rename(Path src, Path dst) throws IOException {
	return nameNodeClient.rename(makeAbsolute(src),makeAbsolute(dst));
    }
	
    // set the working directory
    public void setWorkingDirectory(Path new_dir) {
	try{
	    if(nameNodeClient.exists(makeAbsolute(new_dir)))
		workingDir = makeAbsolute(new_dir);
	}catch(IOException e){
	    return;
	}
    }

    public BlockLocation[] getFileBlockLocations(FileStatus file, long start, long len) throws IOException {
	if (file == null)
	    return null;

	if ( (start<0) || (len < 0) )
	    throw new IllegalArgumentException("Invalid start or len parameter");

	long fsize = file.getLen();
	if (fsize < start)
	    return new BlockLocation[0];

	int blobid = nameNodeClient.blobid(makeAbsolute(file.getPath()));	
	Vector locations = new Vector();
	if (handler.getLatest(blobid))
	    handler.getLocations(start, len, locations);

	Vector<BlockLocation> blocations = new Vector<BlockLocation>();
	Vector<String> names = new Vector<String>(), hosts = new Vector<String>();
	long poffset = start, psize = len;
	for (int i = 0; i < locations.size(); i++) {	    
	    PageLocation p = (PageLocation)locations.elementAt(i);
	    String name = p.host + ":" + p.port;
	    if (names.size() > 0 && (poffset != p.offset || psize != p.size)) {
		blocations.add(new BlockLocation(
						 (String [])names.toArray(new String[0]), 
						 (String [])hosts.toArray(new String[0]), poffset, psize)
		    );
		names.clear();
		hosts.clear();
	    }
	    if (names.size() == 0) {
	        poffset = p.offset;
		psize = p.size;
	    } 
	    if (poffset < fsize) {
		if (poffset + psize > fsize)
		    psize = fsize - poffset;
		names.add(name);
		hosts.add(p.host);
	    }
	}
	if (names.size() > 0)
	    blocations.add(new BlockLocation(
					     (String [])names.toArray(new String[0]), 
					     (String [])hosts.toArray(new String[0]), poffset, psize)
			   );

	return (BlockLocation[])blocations.toArray(new BlockLocation[0]);
    }
	
    // open the file in write mode, ready to add contents at the end
    public FSDataOutputStream append(Path f, int bufferSize, Progressable progress) throws IOException {
	int blobid = nameNodeClient.blobid(makeAbsolute(f));
	FileStatus status = nameNodeClient.status(makeAbsolute(f));
	if(blobid == -1)
	    throw new IOException("Could not find blob associated to " + f);
	else{
	    BSFSCachedOutputStream outputStream = new BSFSCachedOutputStream(blobseerConfig,blobid,(int)this.defaultCacheSize);
		outputStream.setNameNodeClient(nameNodeClient,makeAbsolute(f));
		outputStream.updateStatus();
	    return new FSDataOutputStream(outputStream);
	}
    }
	
    // create a new file
    public FSDataOutputStream create(Path f, FsPermission permission, boolean overwrite, int bufferSize, 
				     short replication, long blockSize, Progressable progress) throws IOException {
	return create(makeAbsolute(f),overwrite, bufferSize, replication, bufferSize, progress);
    }
    
    public boolean createNewFile(Path f) throws IOException {
	  return nameNodeClient.create(makeAbsolute(f), 1, defaultBlockSize);
    }
    
    // private function called to ensure that we work with absolute paths
    private Path makeAbsolute(Path path) {
	Path newPath = path;
	String s = uri.toString();
	String ss = path.toString();
	if (ss.indexOf(s) == 0)
	      newPath = new Path(ss.substring(s.length()));
	if (newPath.isAbsolute()) {
	    return newPath;
	}
	return new Path(workingDir, newPath);
    }
}
