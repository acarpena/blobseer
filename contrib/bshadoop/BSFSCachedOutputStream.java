package org.apache.hadoop.fs.bsfs;
// java import
import java.io.*;
import java.net.*;
import java.util.*;
import java.nio.ByteBuffer;

// hadoop import
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.bsfs.BSFSOutputStream;
import org.apache.hadoop.fs.bsfs.client.BSFSClient;
import blobseer.ObjectHandler;
/*  =================================
 *  Class BSFSCachedOutputStream
 *  describe an object throught which writing in a blob
 *  using a cache system to aligne append.
 *  ================================= */
public class BSFSCachedOutputStream extends BSFSOutputStream {	
    private ByteBuffer cache; // the cache
    private int cacheSize; // size of the cache
    private long fileSize; // size of the file
    private BSFSClient nameNodeClient = (BSFSClient)null; // an instance of the nameNode client
    private Path fileName; // the file name

    // constructor
    public BSFSCachedOutputStream(String cfg, int blobid, int cacheSize) throws IOException {
	super(cfg, blobid);
	this.cacheSize = cacheSize;
	this.cache = ByteBuffer.allocateDirect(cacheSize);
	this.fileSize = 0;
    }
	
    // set the nameNodeClient instance
    public void setNameNodeClient(BSFSClient nameNodeClient, Path filename) {
	this.nameNodeClient = nameNodeClient;
	this.fileName = filename;
    }
	
    // write the full content of a buffer in the blob
    public void write(byte[] b) throws IOException {
	write(b, 0, b.length);
    }
	
    // write part of a buffer in the blob
    public void write(byte[] b, int off, int len) throws IOException {
	if (len <= cache.remaining()) {
	    cache.put(b, off, len);
	    if (cache.remaining() == 0) {
		if (!sendCache()) 
		    throw new IOException("write : unable to append in the blob.");
		updateSize((long)cacheSize);
	    }
	} else {
	    int newLength = cache.remaining();	    
	    cache.put(b, off, newLength);
	    if (!sendCache()) 
		throw new IOException("write : unable to append in the blob.");
	    updateSize((long)cacheSize);
	    write(b, off + newLength, len - newLength);
	}
    }
	
    // write only a byte into the blob
    public void write(int b) throws IOException {
	  if (b >= 256 || b < 0)
//  	    throw new IOException("illegal byte value: " + b);
	    return;
	if (cache.remaining() == 0) {
	    if (!sendCache()) 
		throw new IOException("write : unable to append in the blob.");
	    updateSize((long)cacheSize);
	}
	cache.put((byte)b);
    }
	
    // update the cache
    private boolean sendCache() throws IOException {
	  boolean b = handler.append(cacheSize, cache);
	  cache.rewind();
	  return b;
    }
	
    // flush the cache
    public void flush() throws IOException {
	if (cache.position() > 0) {
	    int p = cache.position();
	    if (!sendCache()) 
		throw new IOException("write : unable to append in the blob.");
	    updateSize((long)p);
	}
    }
	
    // close the stream
    public void close() throws IOException {
	flush();
	super.close();
    }
	
    // update the file size
    public void updateSize(long addSize) throws IOException {
	fileSize += addSize;
	if (!nameNodeClient.setsize(fileName, addSize)) 
	    throw new IOException("updateSize : unable to set the file size on the metadata server.");
    }

    // update the file status
    public void updateStatus() throws IOException {
	FileStatus stats = nameNodeClient.status(fileName);
	if(stats != null)
	    fileSize = stats.getLen();
	else
	    fileSize = 0;
    }
}
