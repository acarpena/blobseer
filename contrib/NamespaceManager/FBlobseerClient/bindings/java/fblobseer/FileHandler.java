package fblobseer;

import java.io.IOException;
import java.util.Vector;
import java.nio.ByteBuffer;

public abstract class FileHandler
{
    // the pointer the C++ FileHandler
    protected long fblob;

    protected final static native boolean fblob_read(long fblob, long offset, long size, ByteBuffer buffer, int version);
    protected final static native boolean fblob_append(long fblob, long size, ByteBuffer buffer);
    protected final static native boolean fblob_write(long fblob, long offset, long size, ByteBuffer buffer);

    protected final static native long fblob_get_size(long fblob, int version);
    protected final static native long fblob_get_size(long fblob);
    protected final static native int fblob_get_latest(long fblob);
    protected final static native int fblob_get_page_size(long fblob);

    static {
        try {
            System.loadLibrary("fbs_namespace-java");
        } catch (UnsatisfiedLinkError e) {
            e.printStackTrace();
            System.err.println("Unable to load libfbs_namespace-java native library");
            System.exit(1);
        }
    }

    // Get the latest version of the file
    public int getLatestVersion() {
    	return fblob_get_latest(fblob);
    }
	
    // Read from the file
    public boolean read(long offset, long size, ByteBuffer buffer, int version) throws IllegalArgumentException {
    	if (!buffer.isDirect())
    		throw new IllegalArgumentException("supplied buffer is not allocated directlty");
    	return fblob_read(fblob, offset, size, buffer, version);
    }

    
    public boolean read(long offset, long size, ByteBuffer buffer) throws IllegalArgumentException {
    	if (!buffer.isDirect())
    		throw new IllegalArgumentException("supplied buffer is not allocated directlty");
    	int version = this.getLatestVersion();
    	return fblob_read(fblob, offset, size, buffer, version);
    }
    
    // Append to the blob
    public boolean append(long size, ByteBuffer buffer) throws IllegalArgumentException {
    	if (!buffer.isDirect())
    		throw new IllegalArgumentException("supplied buffer is not allocated directlty");
    	return fblob_append(fblob, size, buffer);
    }

    // Write into the blob
    public boolean write(long offset, long size, ByteBuffer buffer) throws IllegalArgumentException {
    	if (!buffer.isDirect())
    		throw new IllegalArgumentException("supplied buffer is not allocated directlty");
    	return fblob_write(fblob, offset, size, buffer);
    }

    // Get the size of the blob
    public long getSize(int version) {
    	return fblob_get_size(fblob, version);
    }
    
    public long getSize() {
    	return fblob_get_size(fblob);
    }

    // Get the page size
    public int getPageSize() {
    	return fblob_get_page_size(fblob);
    }

}
