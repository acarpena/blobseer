package org.apache.hadoop.fs.bsfs.client;

import java.io.Serializable;
import java.io.ObjectOutputStream;
import java.io.ObjectInputStream;
import java.io.IOException;
import java.lang.ClassNotFoundException;
import java.util.Vector;

import org.apache.hadoop.fs.Path;

class Info implements Serializable
{
      String path;
      int type;
      long modifTime;
      long pageSize;
      long length;
      int replication;
      
      public Info()
      {}
      
      public Info(String p, int t, long mt, long ps, long l, int r)
      {
	    path = p;
	    type = t;
	    modifTime = mt;
	    pageSize = ps;
	    length = l;
	    replication = r;
      }
      
      public Info(String p, int t, long mt)
      {
	    path = p;
	    type = t;
	    modifTime = mt;
	    pageSize = 0;
	    length = 0;
	    replication = 0;
      }
      
      public String getPath()
      {
	    return path;
      }
      
      public long getModifTime()
      {
	    return modifTime;
      }
      
      public int getType()
      {
	    return type;
      }
      
      public long getPageSize()
      {
	    return pageSize;
      }
      
      public int getReplication()
      {
	    return replication;
      }
      
      public long getLength()
      {
	    return length;
      }
}

public class Reply implements Serializable
{
      int status;
      int blobId;
      String value;
      Vector<Info> infos;
      
      public Reply()
      {
	    infos = new Vector<Info> ();
      }
      
      public int getStatus()
      {
	    return status;
      }
      
      public void setStatus(int s)
      {
	    status = s;
      }
      
      public int getBlobId()
      {
	    return blobId;
      }
      
      public void setBlobId(int b)
      {
	    blobId = b;
      }
      
      public String getValue()
      {
	    return value;
      }
      
      public void setValue(String v)
      {
	    value = v;
      }
      
      public void addInfo(String p, int t, long mt)
      {
	    infos.add(new Info(p, t, mt));
      }
      
      public void addInfo(String p, int t, long mt, long ps, long l, int r)
      {
	    infos.add(new Info(p, t, mt, ps, l, r));
      }
      
      public Vector<Info> getInfo()
      {
	    return infos;
      }
}
