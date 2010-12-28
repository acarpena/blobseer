/**
 * This performs the actual namespace management.
 * 
 * The processRequest is called when a new request must be treated. Input parameters
 * are the byte buffer holding the request and the size of the request. Input data deserialization
 * and interpretation is at user's leisure. The same holds true with output data: expected
 * is a byte array of binary data that can be send directly to the socket as a reply. 
 *
 */

import java.util.HashMap;
import java.util.Iterator;
import java.io.ObjectOutputStream;
import java.io.ObjectInputStream;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import java.io.IOException;

import blobseer.ObjectHandler;

import org.apache.hadoop.fs.bsfs.client.ClientRequest;
import org.apache.hadoop.fs.bsfs.client.Reply;
import org.apache.hadoop.fs.bsfs.client.Constants;

public class Namespace
{
	String BlobSeerConfigFile;
	HashMap<String, MetaData> mappings;
	ObjectHandler handler;
	
	public Namespace(String cfg)
	{
	      BlobSeerConfigFile = cfg;
	      mappings = new HashMap<String, MetaData>();
	      mappings.put(new String("/"), new MetaData(Constants.T_DIR));
	      try {
		    handler = new ObjectHandler(BlobSeerConfigFile);
	      } catch(IOException e){e.printStackTrace();}
	}
	
	public Reply exists(ClientRequest req)
	{
	      Reply rep = new Reply();
	      rep.setStatus(Constants.SUCCESS);
	      if (mappings.get(req.getPath()) == null)
		    rep.setValue("false");
	      else
		    rep.setValue("true");
	      return rep;
	}
	
	public Reply isFile(ClientRequest req)
	{
	      Reply rep = new Reply();
	      MetaData md = mappings.get(req.getPath());
	      if (md == null) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("No such file or directory");
	      }
	      else {
		    rep.setStatus(Constants.SUCCESS);
		    if (md.getType() == Constants.T_FILE)
			  rep.setValue("true");
		    else
			  rep.setValue("false");
	      }
	      return rep;
	}
	
	public Reply mkdirs(ClientRequest req)
	{
	 
	      Reply rep = new Reply();
	      if (mappings.get(req.getPath()) != null) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("Path already exists");
		    return rep;
	      }
	      rep.setStatus(Constants.SUCCESS);
	      String[] dirs = req.getPath().split("/");
	      String prefix = new String();
	      for (int i = 0; i < dirs.length; i++) {
		    if (dirs[i].compareTo("") == 0)
			  continue;
		    prefix = prefix.concat("/" + dirs[i]);
		    MetaData md = mappings.get(prefix);
		    if (md != null) {
			  if (md.getType() == Constants.T_FILE) {
				rep.setStatus(Constants.ERROR);
				rep.setValue("path contains file");
				return rep;
			  }
		    }
		    else
			  mappings.put(prefix, new MetaData(Constants.T_DIR));
	      }
	      rep.setValue("true");
	      return rep;
	}
	
	public Reply rename(ClientRequest req)
	{
	      Reply rep = new Reply();
	      if ((req.getPath().compareTo("/") == 0) || (req.getNewPath().compareTo("/") == 0)) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("cannot rename root directory");
		    return rep;
	      }
	      MetaData md = mappings.get(req.getPath());
	      if (md == null) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("no such file or directory");
		    return rep;
	      }
	      if (mappings.containsKey(req.getNewPath())) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("new name already exists");
		    return rep;
	      }
	      if (md.getType() == Constants.T_FILE) {
		    mappings.remove(req.getPath());
		    md.setModifTime();
		    mappings.put(req.getNewPath(), md);
		    rep.setStatus(Constants.SUCCESS);
		    rep.setValue("true");
		    return rep;
	      }
	      rep.setStatus(Constants.SUCCESS);
	      rep.setValue("true");
	      HashMap<String, MetaData> m = new HashMap<String, MetaData> ();
	      Iterator<String> it = mappings.keySet().iterator();
	      String s = req.getPath();
	      while (it.hasNext()) {
		    try {
			  String p = it.next();
			  if (p.startsWith(s)) {
				MetaData mdd = mappings.get(p);
				mdd.setModifTime();
				String ss = p.replaceFirst(s, req.getNewPath());
				m.put(ss, mdd);
				it.remove();
			  }
		    }
		    catch(Exception e) {e.printStackTrace();}
	      }
	      if (m.size() > 0)
		    mappings.putAll(m);
	      return rep;
	}
	
	public Reply getBlobId(ClientRequest req)
	{
	      Reply rep = new Reply();
	      MetaData md = mappings.get(req.getPath());
	      if (md == null) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("No such file or directory");
		    return rep;
	      }
	      if (md.getType() != Constants.T_FILE) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("Not a file");
		    return rep;
	      }
	      rep.setStatus(Constants.SUCCESS);
	      rep.setBlobId(md.getBlobId());
	      return rep;
	}
	
	public Reply create(ClientRequest req)
	{
	      Reply rep = new Reply();
	      if (mappings.get(req.getPath()) != null) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("file already exists");
		    return rep;
	      }
	      String s = req.getPath();
	      if (s.lastIndexOf('/') != 0) {
		    String ss = s.substring(0, s.lastIndexOf('/'));
		    if (!mappings.containsKey(ss)) {
			  ClientRequest r = new ClientRequest(Constants.MKDIRS);
			  r.setPath(ss);
			  Reply rl = mkdirs(r);
			  if (rl.getStatus() == Constants.ERROR)
				return rl;
		    }
	      }
	      MetaData md = new MetaData(Constants.T_FILE);
	      handler.blobCreate(req.getPageSize(), req.getReplication());
	      md.setBlobId(handler.getId());
	      md.setPageSize(req.getPageSize());
	      md.setReplication(req.getReplication());
	      mappings.put(req.getPath(), md);
	      rep.setStatus(Constants.SUCCESS);
	      rep.setValue("true");
	      return rep;
	}
	
	public Reply getStatus(ClientRequest req)
	{
	      Reply rep = new Reply();
	      MetaData md = mappings.get(req.getPath());
	      if (md == null) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("no such file or directory");
		    return rep;
	      }
	      rep.setStatus(Constants.SUCCESS);
	      rep.setValue("true");
	      if (md.getType() == Constants.T_DIR)
		    rep.addInfo(req.getPath(), Constants.T_DIR, md.getModifTime());
	      else
		    rep.addInfo(req.getPath(), Constants.T_FILE, md.getModifTime(), md.getPageSize(), md.getLength(), md.getReplication());
	      return rep;
	}
	
	public Reply delete(ClientRequest req)
	{
	      Reply rep = new Reply();
	      if (req.getPath().compareTo("/") == 0) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("cannot delete root directory");
		    return rep;
	      }
	      MetaData md = mappings.get(req.getPath());
	      if (md == null) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("no such file or directory");
		    return rep;
	      }
	      if (md.getType() == Constants.T_FILE) {
		    mappings.remove(req.getPath());
		    rep.setStatus(Constants.SUCCESS);
		    rep.setValue("true");
		    return rep;
	      }
	      Iterator<String> it = mappings.keySet().iterator();
	      while (it.hasNext()) {
		    try {
			  String p = it.next();
			  if (p.startsWith(req.getPath()))
				it.remove();
		    }
		    catch(Exception e) {e.printStackTrace();}
	      }
	      rep.setStatus(Constants.SUCCESS);
	      rep.setValue("true");
	      return rep;
	}
	
	public Reply listDir(ClientRequest req)
	{
	      Reply rep = new Reply();
	      MetaData md = mappings.get(req.getPath());
	      if (md == null) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("no such directory");
		    return rep;
	      }
	      if (md.getType() == Constants.T_FILE) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("not a directory");
		    return rep;
	      }
	      rep.setStatus(Constants.SUCCESS);
	      rep.setValue("true");
	      Iterator<String> it = mappings.keySet().iterator();
	      String s = req.getPath();
	      int poz = s.length();
	      if (s.compareTo("/") != 0)
		    poz += 1;
	      while (it.hasNext()) {
		    try {
			  String p = it.next();
			  if (p.compareTo(s) != 0)
				if (p.startsWith(s))
				      if (p.indexOf('/', poz) == -1) {
					    MetaData mdd = mappings.get(p);
					    if (mdd.getType() == Constants.T_FILE)
						  rep.addInfo(p, Constants.T_FILE, mdd.getModifTime(), mdd.getPageSize(), mdd.getLength(), mdd.getReplication());
					    else
						  rep.addInfo(p, Constants.T_DIR, mdd.getModifTime());
					    }
		    }
		    catch(Exception e) {e.printStackTrace();}
	      }
	      return rep;
	}
	
	public Reply setSize(ClientRequest req)
	{
	      Reply rep = new Reply();
	      MetaData md = mappings.get(req.getPath());
	      if (md == null) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("no such file");
		    return rep;
	      }
	      if (md.getType() != Constants.T_FILE) {
		    rep.setStatus(Constants.ERROR);
		    rep.setValue("not a file");
		    return rep;
	      }
	      md.setLength(md.getLength() + req.getLength());
	      md.setModifTime();
	      rep.setStatus(Constants.SUCCESS);
	      rep.setValue("true");
	      return rep;
	}
	
	public byte[] process(byte data[], int size)
	{
	      ClientRequest req = null;
	      try {
		    ObjectInputStream ooi = new ObjectInputStream(new ByteArrayInputStream(data));
		    req = (ClientRequest)ooi.readObject();
	      }
	      catch (Exception e) {e.printStackTrace();}
	      
	      Reply rep = null;
	      int com = req.getCommand();
	      if (com == Constants.MKDIRS)
		    rep = mkdirs(req);
	      if (com == Constants.EXISTS)
		    rep = exists(req);
	      if (com == Constants.ISFILE)
		    rep = isFile(req);
	      if (com == Constants.RENAME)
		    rep = rename(req);
	      if (com == Constants.BLOBID)
		    rep = getBlobId(req);
	      if (com == Constants.CREATE)
		    rep = create(req);
	      if (com == Constants.STATUS)
		    rep = getStatus(req);
	      if (com == Constants.DELETE)
		    rep = delete(req);
	      if (com == Constants.LISTDIR)
		    rep = listDir(req);
	      if (com == Constants.SETSIZE)
		    rep = setSize(req);
	      
	      ByteArrayOutputStream b = new ByteArrayOutputStream();
	      try {
		    ObjectOutputStream oos = new ObjectOutputStream(b);
		    oos.writeObject(rep);
		    oos.close();
	      }
	      catch (Exception e) {e.printStackTrace();}
	      return (b.toByteArray());
	}
}

