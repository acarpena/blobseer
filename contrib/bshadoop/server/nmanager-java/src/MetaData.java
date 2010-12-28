import java.util.Date;

public class MetaData
{
      int type;
      long modifTime;
      int blobId;
      long pageSize;
      long length;
      int replication;
      
      public MetaData(int t)
      {
	    type = t;
	    Date d = new Date();
	    modifTime = d.getTime(); 
      }
      
      public long getModifTime()
      {
	    return modifTime;
      }
      
      public void setModifTime()
      {
	    Date d = new Date();
	    modifTime = d.getTime(); 
      }
      
      public int getType()
      {
	    return type;
      }
      
      public int getBlobId()
      {
	    return blobId;
      }
      
      public void setBlobId(int b)
      {
	    blobId = b;
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
