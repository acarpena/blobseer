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
public class ViewDataElement {
	
	/**
	 * ���̕`��f�[�^�v�f���`��ΏۂƂȂ�ׂ����ǂ���
	 */
	private boolean valid = false;
	
	/**
	 * ���̕`��f�[�^�v�f�̍���(�s�N�Z����)
	 */
	private int height = 0;
	
	/**
	 * ���̕`��f�[�^�v�f�̉��ʒu(���_����̃s�N�Z����)
	 */
	private int pos = 0;

	/**
	 * @return
	 */
	public int getHeight() {
		return height;
	}

	/**
	 * @return
	 */
	public boolean isValid() {
		return valid;
	}

	/**
	 * @return
	 */
	public int getPos() {
		return pos;
	}

	/**
	 * @param i
	 */
	public void setHeight(int i) {
		height = i;
	}

	/**
	 * @param b
	 */
	public void setValid(boolean b) {
		valid = b;
	}

	/**
	 * @param i
	 */
	public void setPos(int i) {
		pos = i;
	}

}
