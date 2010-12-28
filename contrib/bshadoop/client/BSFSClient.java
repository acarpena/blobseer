package org.apache.hadoop.fs.bsfs.client;

// java import

import java.io.IOException;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import java.io.ObjectOutputStream;
import java.io.ObjectInputStream;
import java.io.OutputStream;
import java.io.InputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.*;
// hadoop import
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.FileStatus;
/*  =================================
 *  Class BSFSClient
 *  client of blobseer filesystem : asks for metadata op
 *  ================================= */
public class BSFSClient
{
      private String hostname; // metadata server hostname
      private int port; // metadata server port
      private String uri;
      private Socket clientSocket;
      private InputStream in;
      private OutputStream out;
    
      // constructor
      public BSFSClient(String hostname, int port) throws IOException
      {
	    this.hostname = hostname;
	    this.port = port;
	    this.uri = "bsfs://"+hostname+":"+port;
	    clientSocket = new Socket(hostname, port);
	    in  = clientSocket.getInputStream();
	    out = clientSocket.getOutputStream();
	    if(in == null || out == null) throw new IOException();
      }
      
      public byte[] send(byte[] data) throws IOException 
      {
	    out.write(data);
	    out.flush();
	    
	    byte[] b = new byte[8192];
	    in.read(b);
	    return b;
      }
      
      // private function which send a request and gives the answer of the server
      private Reply sendRequest(ClientRequest req) throws IOException
      {
	    ByteArrayOutputStream b = new ByteArrayOutputStream();
	    try {
		  ObjectOutputStream oos = new ObjectOutputStream(b);
		  oos.writeObject(req);
		  oos.close();
	    }
	    catch (Exception e) {e.printStackTrace();}
	    byte[] bb = send(b.toByteArray());
	    Reply rep = null;
	    try {
		  ObjectInputStream ooi = new ObjectInputStream(new ByteArrayInputStream(bb));
		  rep = (Reply)ooi.readObject();
	    }
	    catch (Exception e) {e.printStackTrace();}
	    return rep;
      }
      
      // mkdirs
      public boolean mkdirs(Path p) throws IOException
      {
	    ClientRequest req = new ClientRequest(Constants.MKDIRS);
	    req.setPath(p.toString());
	    Reply rep = sendRequest(req);
	    if (rep.getStatus() == Constants.ERROR)
		  throw new IOException(rep.getValue());
	    if (rep.getValue().compareTo("false") == 0)
		  return false;
	    return true;
      }
      
      // exists
      public boolean exists(Path p) throws IOException
      {
	    ClientRequest req = new ClientRequest(Constants.EXISTS);
	    req.setPath(p.toString());
	    Reply rep = sendRequest(req);
	    if (rep.getStatus() == Constants.ERROR)
		  return false;
	    if (rep.getValue().compareTo("false") == 0)
		  return false;
	    return true;
      }
      
      // isfile
      public boolean isfile(Path p) throws IOException
      {
	    ClientRequest req = new ClientRequest(Constants.ISFILE);
	    req.setPath(p.toString());
	    Reply rep = sendRequest(req);
	    if (rep.getStatus() == Constants.ERROR)
		  throw new IOException(rep.getValue());
	    if (rep.getValue().compareTo("false") == 0)
		  return false;
	    return true;
      }
      
      // rename
      public boolean rename(Path oldp, Path newp) throws IOException
      {
	    ClientRequest req = new ClientRequest(Constants.RENAME);
	    req.setPath(oldp.toString());
	    req.setNewPath(newp.toString());
	    Reply rep = sendRequest(req);
	    if (rep.getStatus() == Constants.ERROR)
		  throw new IOException(rep.getValue());
	    if (rep.getValue().compareTo("false") == 0)
		  return false;
	    return true;
      }
      
      // blobid
      public int blobid(Path p) throws IOException
      {
	    ClientRequest req = new ClientRequest(Constants.BLOBID);
	    req.setPath(p.toString());
	    Reply rep = sendRequest(req);
	    if (rep.getStatus() == Constants.ERROR)
		  throw new IOException(rep.getValue());
	    return rep.getBlobId();
      }
      
      // create
      public boolean create(Path p, int replica, long psize) throws IOException
      {
	    ClientRequest req = new ClientRequest(Constants.CREATE);
	    req.setPath(p.toString());
	    req.setPageSize(psize);
	    req.setReplication(replica);
	    Reply rep = sendRequest(req);
	    if (rep.getStatus() == Constants.ERROR)
		  throw new IOException(rep.getValue());
	    if (rep.getValue().compareTo("false") == 0)
		  return false;
	    return true;
      }

      // status
      public FileStatus status(Path p) throws IOException
      {
	    ClientRequest req = new ClientRequest(Constants.STATUS);
	    req.setPath(p.toString());
	    Reply rep = sendRequest(req);
	    if (rep.getStatus() == Constants.ERROR)
		  throw new IOException(rep.getValue());
	    if (rep.getValue().compareTo("false") == 0)
		  return null;
	    
	    Vector<Info> v = rep.getInfo();
	    Info inf = v.elementAt(0);
	    if (inf.getType() == Constants.T_DIR)
		  return (new FileStatus(0, true, 1, 0, inf.getModifTime(), p));
	    return (new FileStatus(inf.getLength(), false, inf.getReplication(), inf.getPageSize(), inf.getModifTime(), p));
      }
      
      // detele
      public boolean delete(Path p) throws IOException
      {
	    ClientRequest req = new ClientRequest(Constants.DELETE);
	    req.setPath(p.toString());
	    Reply rep = sendRequest(req);
	    if (rep.getStatus() == Constants.ERROR)
		  throw new IOException(rep.getValue());
	    if (rep.getValue().compareTo("false") == 0)
		  return false;
	    return true;
      }
      
      // listdir
      public FileStatus[] listdir(Path p) throws IOException
      {
	    ClientRequest req = new ClientRequest(Constants.LISTDIR);
	    req.setPath(p.toString());
	    Reply rep = sendRequest(req);
	    if (rep.getStatus() == Constants.ERROR)
		  throw new IOException(rep.getValue());
	    if (rep.getValue().compareTo("false") == 0)
		  return null;
	    
	    Vector<Info> v = rep.getInfo();
	    FileStatus[] rez = new FileStatus[v.size()];
	    for (int i = 0; i < v.size(); i++) {
		  Info inf = v.elementAt(i);
		  if (inf.getType() == Constants.T_DIR)
			rez[i] = new FileStatus(0, true, 1, 0, inf.getModifTime(), new Path(inf.getPath()));
		  else
			rez[i] = new FileStatus(inf.getLength(), false, inf.getReplication(), inf.getPageSize(), inf.getModifTime(), new Path(inf.getPath()));
	    }
	    return rez;
      }
      
      public boolean setsize(Path p, long size) throws IOException
      {
	    ClientRequest req = new ClientRequest(Constants.SETSIZE);
	    req.setPath(p.toString());
	    req.setLength(size);
	    Reply rep = sendRequest(req);
	    if (rep.getStatus() == Constants.ERROR)
		  throw new IOException(rep.getValue());
	    if (rep.getValue().compareTo("false") == 0)
		  return false;
	    return true;
      }
}
