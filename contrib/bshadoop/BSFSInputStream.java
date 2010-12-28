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
import org.apache.hadoop.fs.FSInputStream;
import org.apache.hadoop.fs.bsfs.client.BSFSClient;
import blobseer.ObjectHandler;
/*  =================================
 *  Class BSFSInputStream
 *  describe an object throught which reading in a blob
 *  ================================= */
public abstract class BSFSInputStream extends FSInputStream {	
    protected ObjectHandler handler; // handler for blobseer connection
    protected long position; // current position in the blob
    protected int blobid; // id of the blob to write in
    private BSFSClient nameNodeClient;
    protected long fileSize;
    private Path fileName;

    // update corresponding file status
    public void updateStatus() throws IOException {
	FileStatus status = nameNodeClient.status(fileName);
	if (status != null) {
	    fileSize = status.getLen();
	    handler.getLatest(blobid);
	}
    }

    // constructor
    public BSFSInputStream(String cfg, int blobid, BSFSClient client, Path filename) throws IOException {
	handler = new ObjectHandler(cfg);
	position = 0;
	this.blobid = blobid;
	this.nameNodeClient = client;
	this.fileName = filename;
	updateStatus();
    }
	
    // fill the buffer with the content of the blob starting at pos
    public void readFully(long pos, byte[] buffer) throws IOException {
	int readSize = buffer.length;
	ByteBuffer b = ByteBuffer.allocateDirect(readSize);
	if (pos + readSize > fileSize)
	    updateStatus();
	if (pos + readSize > fileSize)
	    throw new IOException("readFully: read attempt beyond file size");
	if(!handler.read(pos, readSize, b)) 
	    throw new IOException("readFully: error while reading the blob.");
	b.get(buffer);
    }
}
