package org.apache.hadoop.fs.bsfs.client;

import java.io.Serializable;
import java.io.ObjectOutputStream;
import java.io.ObjectInputStream;
import java.io.IOException;
import java.lang.ClassNotFoundException;

import org.apache.hadoop.fs.Path;

public class ClientRequest implements Serializable
{
      int command;
      String path;
      String newPath;
      long pageSize;
      long length;
      int replication;
      
      public ClientRequest()
      {
	    command = 0;
	    path = null;
	    newPath = null;
	    pageSize = 0;
	    length = 0;
	    replication = 0;
      }
      
      public ClientRequest(int c)
      {
	    command = c;
	    path = null;
	    newPath = null;
	    pageSize = 0;
	    length = 0; 
	    replication = 0;
      }
      
      public int getCommand()
      {
	    return command;
      }
      
      public String getPath()
      {
	    return path;
      }
      
      public void setPath(String p)
      {
	    path = p;
      }
      
      public String getNewPath()
      {
	    return newPath;
      }
      
      public void setNewPath(String p)
      {
	    newPath = p;
      }
      
      public long getPageSize()
      {
	    return pageSize;
      }
            
      public void setPageSize(long p)
      {
	    pageSize = p;
      }
      
      public int getReplication()
      {
	    return replication;
      }
      
      public void setReplication(int r)
      {
	    replication = r;
      }
      
      public long getLength()
      {
	    return length;
      }
      
      public void setLength(long l)
      {
	    length = l;
      }
}
