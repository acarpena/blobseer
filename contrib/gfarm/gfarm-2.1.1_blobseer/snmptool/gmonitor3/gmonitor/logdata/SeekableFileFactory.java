/*
 * Created on 2003/06/20
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package gmonitor.logdata;

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;

/**
 * @author hkondo
 *
 * To change the template for this generated type comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class SeekableFileFactory {
	
	/**
	 * URL u �Ŏ������t�@�C����p���� SeekableFile �𐶐�����B
	 * u �̃v���g�R���� file: �ł���� LocalSeekableFile ���Ahttp: �ł���� HttpSeekableFile�@��
	 * ���������B����ȊO�̃v���g�R���ł� MalformedURLException ����������
	 * @param u SeekableFile������ URL
	 * @return u �Ŏ����ꂽ�t�@�C�����琶�����ꂽ SeekableFile
	 * @throws IOException
	 * @throws MalformedURLException u �̃v���g�R����file:��������http:�ȊO�ł������ꍇ�B
	 */
	public static SeekableFile create(URL u) throws IOException {
		SeekableFile file = null;
		String p = u.getProtocol();
		if(p.equalsIgnoreCase("file")){
			// file: �Ȃ̂� LocalSeekableFile �𐶐�
			File f = new File(u.getPath());
			file = new LocalSeekableFile(f);
		}else if(p.equalsIgnoreCase("http")){
			// http: �Ȃ̂� HttpSeekableFile �𐶐�
			file = new HttpSeekableFile(u);
		}else{
			// ���m�̃v���g�R��
			throw new MalformedURLException("Unsupported Protocol" + p);
		}
		return file;
	}
	
	public static SeekableFile create(File f) throws IOException {
		// File �ł���킳���t�@�C�������� SeekableFile �𐶐�����
		return new LocalSeekableFile(f);
	}

	public static SeekableFile create(String url) throws IOException {
		SeekableFile file = null;
		if(url.startsWith("http:")){
			URL u = new URL(url);
			file = create(u);
		}else{
			File f = new File(url);
			file = create(f);
		}
		return file;
	}
}
