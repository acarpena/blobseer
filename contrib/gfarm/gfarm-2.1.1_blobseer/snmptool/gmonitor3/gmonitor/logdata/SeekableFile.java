/*
 * Created on 2003/06/20
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package gmonitor.logdata;

import java.io.IOException;

/**
 * @author hkondo
 *
 * To change the template for this generated type comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public interface SeekableFile {

	public void close() throws IOException;

	/**
	 * @return
	 * @throws java.io.IOException
	 */
	public long size() throws IOException;

	/**
	 * @param buf �ǂݏo���o�b�t�@
	 * @param idx �o�b�t�@���̊J�n�ʒu
	 * @param amount �ǂݏo����
	 * @return
	 * @throws java.io.IOException
	 */
	public int read(byte[] buf, int idx, int amount) throws IOException;
	
	public void seek(long pos) throws IOException;

}
