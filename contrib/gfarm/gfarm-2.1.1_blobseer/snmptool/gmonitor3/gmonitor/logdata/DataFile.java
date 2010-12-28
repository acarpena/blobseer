package gmonitor.logdata;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;

/*
 * Created on 2003/05/14
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */

/**
 * @author hkondo
 *
 * To change the template for this generated type comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class DataFile implements Comparable {
	private String url;
	private SeekableFile file;
	private FirstMetaBlock  fmb;
	private SecondMetaBlock smb;
	private long latestDateTime = -1;
	private long size;
	private int dbgroup_length; // �t�@�C�����Ɋ܂܂��f�[�^�u���b�N�O���[�v�̌�
	long datablock_pos;
	private static final int CAPACITY = 100;
	private static HashMap instanceMap = new HashMap();
	private static ArrayList instanceGeneration = new ArrayList();

	public static DataFile getInstance(String url) throws IOException {
		DataFile f = null;
		synchronized(instanceMap){
			Object o = instanceMap.get(url);
			if(o == null){
				// first accessing to url
				f = new DataFile(url);
				instanceMap.put(url, f);
				instanceGeneration.add(url);
				if(instanceGeneration.size() > CAPACITY){
					// �����ɃI�[�v�����Ă����t�@�C�����𒴉߂����̂ŃN���[�Y����
					String u = (String) instanceGeneration.remove(0);
					DataFile df = (DataFile) instanceMap.remove(u);
					df.close();
				}
			}else{
				f = (DataFile) o;
				int idx = instanceGeneration.indexOf(url);
				// �A�N�Z�X���ꂽ�̂Ő�������t���b�V��
				instanceGeneration.remove(idx);
				instanceGeneration.add(url);
				
				DataFile df = (DataFile) instanceMap.remove(url);
				df.close();
				f = new DataFile(url);
				instanceMap.put(url, f);
			}
		}
		return f;
	}
	/**
	 * 
	 */
	public void close() throws IOException {
		file.close();
	}
	public void reload() throws IOException
	{
//System.out.println("Reloaded file attr: " + getUrl());
		initialize();
	}
	protected void initialize() throws IOException
	{
		file.seek(0);

		byte[] sz = new byte[2];
		file.read(sz, 0, 2);
		int size = UTY.byte2int(sz);
		
		byte[] fmbBytes = new byte[size];
		file.read(fmbBytes, 0, size);
		InputStream is = new ByteArrayInputStream(fmbBytes, 0, size);
		fmb = (FirstMetaBlock)FirstMetaBlock.newInstance(is, size);
		is.close();
		
		byte[]dsz = new byte[4];
		file.read(dsz, 0, 4);
		size = UTY.byte2int(dsz);
		byte[] smbBytes = new byte[size];
		file.read(smbBytes, 0, size);
		is = new ByteArrayInputStream(smbBytes, 0, size);
		smb = (SecondMetaBlock)SecondMetaBlock.newInstance(is, size);
		is.close();
		this.size = file.size();

		// �t�@�C���T�C�Y����A�܂܂��ł��낤�f�[�^�u���b�N�O���[�v�̌����Z�o���Adbgroup_length�Ɋo����B
		DataBlockGroupTable t = smb.getDataBlockGroupTable();
		ArrayList l = t.getDataBlockGroupElements();
		datablock_pos = fmb.getSize() + smb.getSize() + 2 + 4; // 2 + 4 means size fields.
		dbgroup_length = (int) ((this.size - datablock_pos) / fmb.getDataBlockGroupSize());
		latestDateTime = -1L;
		
		file.seek(datablock_pos);
	}
	protected DataFile(String url) throws IOException{
		this.url = url;
		SeekableFile f = SeekableFileFactory.create(url);
//		file = new CachedSeekableFile(f); // TODO: CachedSeekableFile
file = f;
		initialize();
	}

	/**
	 * �t�@�C���Ɋ܂܂�Ă��銮�S�ȃf�[�^�u���b�N�O���[�v�̌���Ԃ�
	 * @return
	 */
	public long getLength()
	{
		return dbgroup_length;
	}
	
	/**
	 * @return
	 */
	public FirstMetaBlock getFirstMetaBlock() {
		return fmb;
	}
	/**
	 * @return
	 */
	public SecondMetaBlock getSecondMetaBlock() {
		return smb;
	}

	public DataBlockGroup getDataBlockGroup(long idx) throws IOException
	{
		int i = 0;
		DataBlockGroup dbg = new DataBlockGroup();
		int szGroup = fmb.getDataBlockGroupSize();
		byte[] buf = new byte[szGroup];

		// 1.�ǂݏo���ׂ��ʒu�܂� seek ����
		long pos = datablock_pos + (idx * szGroup);
		file.seek(pos);
		
		// 2.read ����
		int ret = file.read(buf, 0, szGroup);
		if(ret < 0){
			// �t�@�C���̏I�[���z�����B
			throw new IOException("File pointer exceeded.");
		}
		
		// 3.parse ����
		DataBlockGroupTable dbgt = smb.getDataBlockGroupTable();
		ArrayList rows = dbgt.getDataBlockGroupElements();
		for(int r = 0; r < rows.size(); r++){
			DataBlockGroupElement[] row = (DataBlockGroupElement[]) rows.get(r);
			if(row.length > 0){
				int dbsz = (4 + 4) + 5 * row.length;
				DataBlock db = new DataBlock(buf, i, row.length);
				dbg.add(db);
				i += dbsz;
			}else{
				DataBlock db = new DataBlock();
				dbg.add(db);
			}
		}
		
		return dbg;
	}

	/**
	 * ���̃f�[�^�t�@�C���ɂ���������Ƃ��Â��v���l�̎������擾����
	 * @return ����
	 */
	public long getBeginDateTime()
	{
		return fmb.getBeginDate();
	}

	/**
	 * ���̃f�[�^�t�@�C�����̂����Ƃ��V�����v���l�̎������擾����
	 * @return ����
	 */
	public long getLatestDateTime() throws IOException
	{
		if(latestDateTime > 0){
			// Already computed.
			return latestDateTime;
		}
		long latest = fmb.getBeginDate();
		// 1.get the latest datablock group.
		ArrayList dbg = getDataBlockGroup(dbgroup_length - 1);

		// 2.scan datablock group to determine the latest time point.
		for(int i = 0; i < dbg.size(); i++){
			DataBlock db = (DataBlock) dbg.get(i);
			if(db.isValid() == true){
				long t = db.getTime();
				if(t > latest){
					latest = t;
				}
			}else{
				// db is invalid, skip it.
			}
		}
		
		// 3.cache latest time and return.
		latestDateTime = latest;
		return latest;
	}
	
	/**
	 * ���̃f�[�^�t�@�C�����ɁA�w�肳�ꂽ�z�X�g���܂܂�Ă��邩�ǂ������e�X�g����
	 * @param host ��������z�X�g��
	 * @return �܂܂�Ă���� true.
	 */
	public boolean containsHost(String host)
	{
		return smb.containsHost(host);
	}
	
	/**
	 * ���̃f�[�^�t�@�C�����ɁA�w�肳�ꂽ�C�x���g���܂܂�Ă��邩�ǂ������e�X�g����
	 * @param event ��������C�x���g��
	 * @return �܂܂�Ă���� true.
	 */
	public boolean containsEvent(String event) {
		return smb.containsEvent(event);
	}

	/**
	 * ���̃f�[�^�t�@�C�����ɁA�w�肳�ꂽ�������܂܂�Ă��邩�ǂ������e�X�g����
	 * �������A�u�܂��ɂ��傤�ǂ��̎����v���܂܂�邩�ǂ����͖��ɂ��Ȃ��B
	 * @param t �������鎞��
	 * @return �܂܂�Ă���� true.
	 */
	public boolean containsDateTime(long t) throws IOException {
		boolean ret = false;
		long b = getBeginDateTime();
		long e = getLatestDateTime();
		if(b <= t && t <=e){
			ret = true;
		}
		return ret;
	}
	
	/**
	 * �Ƃ��鎞�����ǂ̃f�[�^�u���b�N�O���[�v�Ɋ܂܂�Ă��邩���v�Z����
	 * @param t �e�X�g����������
	 * @return �f�[�^�u���b�N�O���[�v�̃C���f�N�X
	 */
	public long getDataBlockGroupIndex(long t) throws IOException
	{
		long idx = -1;
		if(containsDateTime(t) == false){
			// ���̃t�@�C���͈̔͊O�̎������w�肳��Ă���̂ŃG���[
			throw new IOException("TimePointOutOfRangeInFile " + getUrl() + " " + t);
		}
		long intv = fmb.getGroupInterval();
		long begin = getBeginDateTime();
		idx = (long) (t - begin) / (long) intv;
		return idx;		
	}

	/* (non-Javadoc)
	 * @see java.lang.Comparable#compareTo(java.lang.Object)
	 */
	public int compareTo(Object o) {
		if((o instanceof DataFile) == false){
			// �N���X���Ⴄ�̂Ŕ�r�ł��Ȃ��B
			throw new ClassCastException("Object is not compatible.");
		}
		DataFile f = (DataFile) o;
		long t1 = getBeginDateTime();
		long t2 = f.getBeginDateTime();
		return (int) (t1 - t2);
	}
	/**
	 * @return
	 */
	public String getUrl() {
		return url;
	}

	public DataBlock readDataBlock(long idx) throws IOException
	{
		// �ʂ��ԍ��� idx �̃f�[�^�u���b�N���擾����Bidx �� 0�@����n�܂�B
		DataBlockGroupTable tbl = smb.getDataBlockGroupTable();
		long blockCount = tbl.getDataBlockGroupElements().size();
		long dbg_idx = idx / blockCount;
		DataBlockGroup dbg = null;
		try {
			dbg = getDataBlockGroup(dbg_idx);
		} catch (IOException e) {
			// �Y������f�[�^�u���b�N�O���[�v�͑��݂��Ȃ��B
			return null;
		}
		int db_idx = (int) (idx % blockCount);
		if(db_idx >= dbg.size()){
			// �Y������f�[�^�u���b�N�͑��݂��Ȃ��B
			return null;
		}
		DataBlock db = (DataBlock) dbg.get(db_idx);
		return db;
	}
}
