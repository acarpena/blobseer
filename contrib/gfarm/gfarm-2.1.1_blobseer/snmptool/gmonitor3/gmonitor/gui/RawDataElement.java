package gmonitor.gui;
/*
 * Created on 2003/06/04
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
public class RawDataElement {
	
	/**
	 * ���̗v�f�������v���|�C���g�̌v���l���L�����ǂ���
	 */
	private boolean valid = false;
	
	/**
	 * ���̗v�f�������v���|�C���g�̌v���l
	 * valid �� false �ł������ꍇ�́A�Ӗ��̂���l�Ƃ��ĎQ�Ƃ��Ă͂Ȃ�Ȃ�
	 */
	private long value = 0L;
	
	/**
	 * ���̗v�f�������v���|�C���g�̎���
	 * valid �������Ȃ�l�ł����Ă��A�����͐��������̂Ƃ��ĎQ�Ƃ��Ă悢
	 */
	private long time = 0L;

	/**
	 * @return
	 */
	public boolean isValid() {
		return valid;
	}

	/**
	 * @return
	 */
	public long getValue() {
		return value;
	}

	/**
	 * @param b
	 */
	public void setValid(boolean b) {
		valid = b;
	}

	/**
	 * @param l
	 */
	public void setValue(long l) {
		value = l;
	}

	/**
	 * @return
	 */
	public long getTime() {
		return time;
	}

	/**
	 * @param l
	 */
	public void setTime(long l) {
		time = l;
	}

}
