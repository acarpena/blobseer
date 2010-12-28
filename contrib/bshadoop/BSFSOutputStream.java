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
import java.nio.ByteBuffer;
// hadoop import
import blobseer.ObjectHandler;
/*  =================================
 *  Class BSFSOutputStream
 *  describe an object throught which writing in a blob
 *  ================================= */
public abstract class BSFSOutputStream extends OutputStream {

	protected ObjectHandler handler; // handler for blobseer connection
	protected int blobid; // id of the blob to write in
	
	// constructor
	public BSFSOutputStream(String cfg, int blobid) throws IOException {
		handler = new ObjectHandler(cfg);
		handler.getLatest(blobid);
		this.blobid = blobid;
	}
}
