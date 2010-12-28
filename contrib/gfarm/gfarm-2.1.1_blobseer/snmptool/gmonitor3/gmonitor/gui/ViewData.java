package gmonitor.gui;
import java.awt.Color;

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
public class ViewData {
	
	/**
	 * ���̕`��f�[�^��`��ΏۂƂ��Ă悢���ǂ���
	 */
	private boolean valid = false;
	
	/**
	 * ���̕`��f�[�^����ʏ�ŏ����ȓ_(�v���b�g)�Ƃ��ĕ`�悷�ׂ����ǂ���
	 */
	private boolean plot = false;
	
	/**
	 * ���̕`��f�[�^�̃v���b�g��`�悷�ׂ��F
	 */
	private Color plotColor = Color.GREEN;
	
	/**
	 * ���̕`��f�[�^���琂���������ɓh��Ԃ����s���ׂ����ǂ���(�_�O���t�̂悤�Ɍ�����)
	 */
	private boolean fill = false;
	
	/**
	 * ���̕`��f�[�^�𐂒��������ɓh��Ԃ��ׂ��F 
	 */
	private Color fillColor = Color.RED;
	
	/**
	 * ���̕`��f�[�^�Ɨאڂ���L���ȕ`��f�[�^�Ƃ�����ŘA�����ĕ`�悷�ׂ����ǂ���
	 */
	private boolean join = false;
	
	/**
	 * �`��f�[�^��A�����������`�悷�ׂ��F
	 */
	private Color joinColor = Color.YELLOW;
	
	/**
	 * �ŐV�f�[�^������������(���x�����[�^)��`�悷�ׂ����ǂ���
	 */
	private boolean level = false;
	
	/**
	 * ���x�����[�^��`�悷�ׂ��F
	 */
	private Color levelColor = Color.BLUE;
	
	/**
	 * �`��f�[�^�v�f�̔z��
	 */
	private ViewDataElement[] data;
	
	/**
	 * �v���b�g�̔��a(�s�N�Z���P��)
	 */
	private int plotRadius = 2;


	/**
	 * @return
	 */
	public ViewDataElement[] getData() {
		return data;
	}

	/**
	 * @return
	 */
	public boolean isFill() {
		return fill;
	}

	/**
	 * @return
	 */
	public Color getFillColor() {
		return fillColor;
	}

	/**
	 * @return
	 */
	public boolean isJoin() {
		return join;
	}

	/**
	 * @return
	 */
	public Color getJoinColor() {
		return joinColor;
	}

	/**
	 * @return
	 */
	public boolean isLevel() {
		return level;
	}

	/**
	 * @return
	 */
	public Color getLevelColor() {
		return levelColor;
	}

	/**
	 * @return
	 */
	public boolean isPlot() {
		return plot;
	}

	/**
	 * @return
	 */
	public Color getPlotColor() {
		return plotColor;
	}

	/**
	 * @return
	 */
	public boolean isValid() {
		return valid;
	}

	/**
	 * @param elements
	 */
	public void setData(ViewDataElement[] elements) {
		data = elements;
	}

	/**
	 * @param b
	 */
	public void setFill(boolean b) {
		fill = b;
	}

	/**
	 * @param color
	 */
	public void setFillColor(Color color) {
		fillColor = color;
	}

	/**
	 * @param b
	 */
	public void setJoin(boolean b) {
		join = b;
	}

	/**
	 * @param color
	 */
	public void setJoinColor(Color color) {
		joinColor = color;
	}

	/**
	 * @param b
	 */
	public void setLevel(boolean b) {
		level = b;
	}

	/**
	 * @param color
	 */
	public void setLevelColor(Color color) {
		levelColor = color;
	}

	/**
	 * @param b
	 */
	public void setPlot(boolean b) {
		plot = b;
	}

	/**
	 * @param color
	 */
	public void setPlotColor(Color color) {
		plotColor = color;
	}

	/**
	 * @param b
	 */
	public void setValid(boolean b) {
		valid = b;
	}

	/**
	 * @return
	 */
	public int getPlotRadius() {
		return plotRadius;
	}

	/**
	 * @param i
	 */
	public void setPlotRadius(int i) {
		plotRadius = i;
	}

}
