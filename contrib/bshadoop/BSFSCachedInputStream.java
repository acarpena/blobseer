package org.apache.hadoop.fs.bsfs;
/* =================================
 * BlobSeer FileSystem for Hadoop
 * this file is part of the work conducted in KerData
 * team (IRISA - Rennes, Fr)
 * ================================= */
// java import
import java.io.*;
import java.nio.ByteBuffer;
// hadoop import
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.bsfs.BSFSOutputStream;
import org.apache.hadoop.fs.bsfs.client.BSFSClient;
/*  =================================
 *  Class BSFSCachedInputStream
 *  describe an object throught which reading in a blob
 *  with an optimised access using a cache
 *  ================================= */
public class BSFSCachedInputStream extends BSFSInputStream {
    private ByteBuffer cache;
    private long cacheOffset;
    private int cacheSize;
	
    // constructor
    public BSFSCachedInputStream(String cfg, int blobid, long csize, BSFSClient client, Path filename) throws IOException {
	super(cfg, blobid, client, filename);
        cacheSize = (int)csize;
	updateStatus();
	if (fileSize < cacheSize)
	    cacheSize = (int)fileSize;

	cache = ByteBuffer.allocateDirect(cacheSize);
	cache.rewind();
	cache.limit(0);
	cacheOffset = 0;
    }
	
    // check if we hit on the current cache
    private int cacheHit(int size) {
	int remaining = cache.remaining();
	if (cache.remaining() > size)
	    return size;
	else
	    return cache.remaining();
    }
	
    // read only one byte at the current position
    public int read() throws IOException {
	if (cacheHit(1) > 0)
	    return cache.get();
	else {
	    if (!updateCache())
		return -1;
	    return cache.get();
	}
    }


    // get the current position
    public long getPos() {
	return cacheOffset + cache.position();
    }
	
    // seek to a new position
    public void seek(long pos) throws IOException {
	if (pos > fileSize)
	    updateStatus();
	if (pos > fileSize) 
	    throw new IOException("seek: position given greater than blob size.");	    
 
	if ((pos >= cacheOffset) && (pos < (cacheOffset + cache.limit()))) {
	    cache.position((int)(pos - cacheOffset));
	    return;
	}
	while (cacheOffset < pos)
	    cacheOffset += cacheSize;
	while (cacheOffset > pos)
	    cacheOffset -= cacheSize;
	cache.rewind();
	cache.limit(0);
    }
    
    // seek to a new position
    public boolean seekToNewSource(long targetPos) throws IOException {
	seek(targetPos);
	return true;
    }

    // perform a read operation
    public int read(byte[] b, int off, int len) throws IOException, NullPointerException {
	if (len == 0) 
	    return 0;
	if (b == null)
	    throw new NullPointerException("BSFSCachedInputStream::read: Null buffer supplied");

	int reminder = len;
	int new_off = off;
	while (true) {
	    int in_cache = cacheHit(reminder);
	    if (in_cache > 0) {
		cache.get(b, new_off, in_cache);
		new_off += in_cache;
		position += in_cache;
		reminder -= in_cache;
	    }
	    if (reminder == 0)
		break;
	    if (!updateCache())
		break;
	}
	if (len == reminder)
	    return -1;
	return (len - reminder);
    }
	
    // update the content of the cache
    private boolean updateCache() throws IOException {
	int readSize = cacheSize;
	long blobPosition = cacheOffset + cache.position();
	if (blobPosition >= fileSize)
	    return false;
	if ((blobPosition + readSize) > fileSize)
	    readSize = (int)(fileSize - blobPosition);
	if(!handler.read(blobPosition, readSize, cache))
	    throw new IOException("read: error while reading the blob.");
	cacheOffset = blobPosition;
	cache.rewind();
	cache.limit(readSize);
	return true;
    }
}
