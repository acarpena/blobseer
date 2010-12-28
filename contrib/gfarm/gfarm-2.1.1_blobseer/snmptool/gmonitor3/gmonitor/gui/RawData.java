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
public class RawData {
	
	private Color plotColor = Color.GREEN;
	private Color lineColor = Color.BLUE;
	private Color barColor = Color.RED;
	private Color levelColor = Color.YELLOW;

	/**
	 * ���̌v���f�[�^�n��̂����Ƃ��Â��f�[�^�̓���(Java����)
	 */
//	private long epoch; // XXXtodoXXX �v�čl: �����ɂ���ׂ��ł͂Ȃ��悤�ȋC������

	/**
	 * ���̃I�u�W�F�N�g���ێ����Ă���v���f�[�^�̍ŏ��̓���(Java����)
	 */
//	private long begin;	// XXXtodoXXX �v�čl: �����ɂ���ׂ��ł͂Ȃ��悤�ȋC������
	
	/**
	 * ���̃I�u�W�F�N�g���ێ����Ă���v���f�[�^�̌^(�������Ƃ��Ĉ������ǂ���)
	 */
	private boolean diffmode = false;

	/**
	 * ���̊ϑ��Ώۂɂ����āASNMP �J�E���^�[�l�̐܂�Ԃ����������鐔�l
	 */
	private long max = 4294967295L; // SNMP int32 max.

	/**
	 * ���̌v���f�[�^�n��̑Ώۃz�X�g��
	 */
//	private String host;

	/**
	 * ���̌v���f�[�^�n��̑Ώێ��ۖ�
	 */
//	private String event;
	
	/**
	 * ���̌v���f�[�^�n��̌v���Ԋu(in milli-seconds)
	 */
//	private long interval;
	
	/**
	 * ���̌v���f�[�^�̃q���g��
	 */
//	private String hint;
	
	/**
	 * ���̌v���f�[�^�������ΏۂƂ��Đ�������Ԃɂ��邩�ǂ���
	 */
	private boolean valid;
	
	/**
	 * �v���f�[�^���̂���
	 */
	private RawDataElement[] data;

	/**
	 * @return
	 */
//	public long getBegin() {
//		return begin;
//	}

	/**
	 * @return
	 */
	public RawDataElement[] getData() {
		return data;
	}

	/**
	 * @return
	 */
	public boolean isDiffmode() {
		return diffmode;
	}

	/**
	 * @return
	 */
//	public long getEpoch() {
//		return epoch;
//	}

	/**
	 * @return
	 */
//	public String getEvent() {
//		return event;
//	}

	/**
	 * @return
	 */
//	public String getHint() {
//		return hint;
//	}

	/**
	 * @return
	 */
//	public String getHost() {
//		return host;
//	}

	/**
	 * @return
	 */
//	public long getInterval() {
//		return interval;
//	}

	/**
	 * @return
	 */
	public long getMax() {
		return max;
	}

	/**
	 * @return
	 */
	public boolean isValid() {
		return valid;
	}

	/**
	 * @param l
	 */
//	public void setBegin(long l) {
//		begin = l;
//	}

	/**
	 * @param elements
	 */
	public void setData(RawDataElement[] elements) {
		data = elements;
	}

	/**
	 * @param b
	 */
	public void setDiffMode(boolean b) {
		diffmode = b;
	}

	/**
	 * @param l
	 */
//	public void setEpoch(long l) {
//		epoch = l;
//	}

	/**
	 * @param string
	 */
//	public void setEvent(String string) {
//		event = string;
//	}

	/**
	 * @param string
	 */
//	public void setHint(String string) {
//		hint = string;
//	}

	/**
	 * @param string
	 */
//	public void setHost(String string) {
//		host = string;
//	}

	/**
	 * @param l
	 */
//	public void setInterval(long l) {
//		interval = l;
//	}

	/**
	 * @param l
	 */
	public void setMax(long l) {
		max = l;
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
	public Color getBarColor() {
		return barColor;
	}

	/**
	 * @return
	 */
	public Color getLineColor() {
		return lineColor;
	}

	/**
	 * @return
	 */
	public Color getPlotColor() {
		return plotColor;
	}

	/**
	 * @param color
	 */
	public void setBarColor(Color color) {
		barColor = color;
	}

	/**
	 * @param color
	 */
	public void setLineColor(Color color) {
		lineColor = color;
	}

	/**
	 * @param color
	 */
	public void setPlotColor(Color color) {
		plotColor = color;
	}

	/**
	 * @return
	 */
	public Color getLevelColor() {
		return levelColor;
	}

	/**
	 * @param color
	 */
	public void setLevelColor(Color color) {
		levelColor = color;
	}

}
