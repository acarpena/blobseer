/*
 * Created on 2003/07/29
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package gmonitor.tools;

import gmonitor.gui.RawData;
import gmonitor.gui.RawDataElement;

import java.util.ArrayList;

/**
 * @author hkondo
 *
 * To change the template for this generated type comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class SumRawDataSeries {

	public RawDataElement[] total(RawData[] rda)
	{
		RawData newData = new RawData();
		ArrayList newList = new ArrayList();
		long time = 0;
		int series = rda.length;

		RawDataElement[] totalBuffer = new RawDataElement[series];
		int[] scanIndex = new int[series];
		long endTime[] = new long[series];
		fillEndTime(endTime, rda);

		while(true){
			// totalBuffer ���X�V���Ė߂�B
			time = scanNext(time, rda, scanIndex, totalBuffer);
	
			// ���Z�o�b�t�@�����v���A�V���ȗv�f�Ƃ���
			long v = addup(totalBuffer);
			RawDataElement ne = new RawDataElement();
			ne.setTime(time);
			ne.setValue(v);
			newList.add(ne);
	
			// �I�������H
			if(isFinishedTime(time, endTime) == true){
				break;
			}
		}
		
		RawDataElement[] rde = new RawDataElement[newList.size()];
		rde = (RawDataElement[]) newList.toArray(rde);
		return rde;
	}

	private long scanNext(long time, RawData[] rda, int[] si, RawDataElement[] tb)
	{
		int series = rda.length;

		// ���ׂĂ̌n��ɂ��āA
		long t = Long.MAX_VALUE;
		for(int s = 0; s < series; s++){
			RawDataElement[] rde = rda[s].getData();
			// time ���傫���v�f�̃C���f�N�X��T���B
			int idx = findFirstExceededTime(time, rde);
			// ���̒��ŁA�ŌẪ^�C���X�^���v�̗v�f���u�}�b�`�����v�Ƃ���
			if(idx < 0){
				// ���̌n��ł͌�����Ȃ������B
			}else{
				// �Ƃ肠�����o���Ă����B
				si[s] = idx;
				long tt = rde[idx].getTime();
				if(t < tt){
					t = tt;
				}
			}
		}

		// �}�b�`�����v�f��tb�Ɋo����B�����}�b�`�����ꍇ�ɂ͂��ׂĊo����B
		for(int s = 0; s < series; s++){
			RawDataElement[] rde = rda[s].getData();
			if(rde[si[s]].getTime() == t){
				// ���Z�o�b�t�@�ɒ~����ׂ��B
				tb[s] = rde[si[s]];
			}
		}
		return t;
	}

	/**
	 * @param time
	 * @param rde
	 * @return
	 */
	private int findFirstExceededTime(long time, RawDataElement[] rde) {
		for(int i = 0; i < rde.length; i++){
			if(rde[i].isValid() == true){
				if(rde[i].getTime() > time){
					return i;
				}
			}
		}
		return -1;
	}

	private boolean isFinishedTime(long t, long[] endTime)
	{
		for(int i = 0; i < endTime.length; i++){
			if(t < endTime[i]){
				return false;
			}
		}
		// t >= endTime[all of];
		return true;
	}

	private void fillEndTime(long[] endTime, RawData[] rda)
	{
		for(int i = 0; i < rda.length; i++){
			RawDataElement[] rde = rda[i].getData();
			endTime[i] = rde[rde.length + 1].getTime();
		}
		return;
	}
	
	private long addup(RawDataElement[] rde)
	{
		long s = 0;
		for(int i = 0; i < rde.length; i++){
			s += rde[i].getValue();
		}
		return s;
	}


}
