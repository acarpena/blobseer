/*
 * Created on 2003/07/09
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package gmonitor.logdata;

import java.util.ArrayList;
import java.util.Date;

/**
 * @author hkondo
 *
 * To change the template for this generated type comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public class DataBlockGroup extends ArrayList {

	// �����Ŗ߂���� ArrayList �́A�K�� java.util.Date �� DataElement ���g�ɂȂ��Ă���B
	public ArrayList pickData(DataBlockGroupTable tbl, int hidx, int oidx)
	{
		// hidx �� oidx �Ƀ}�b�`����f�[�^���������Ď��o���B
		ArrayList ret = new ArrayList();
		ArrayList rows = tbl.getDataBlockGroupElements();
		synchronized(this){
			for(int i = 0; i < rows.size(); i++){
				DataBlockGroupElement[] row = (DataBlockGroupElement[]) rows.get(i);
				for(int j = 0; j < row.length; j++){
					if(row[j].isPairOfHIDandOID(hidx, oidx) == true){
						// ���� row �Ƀy�A�𔭌�����
						DataBlock db = (DataBlock) this.get(i);
						long t = db.getTime();
						Date dt = new Date(t);
						ArrayList delist = db.getData();
						DataElement de = (DataElement) delist.get(j);
						ret.add(dt);
						ret.add(de);
						// row�@�ɂ͕K���ЂƂ̃f�[�^�����Ȃ��B��ȏ㑶�݂��Ă��A����������ł���
						// �ȏ�A�����v���l�łȂ���΂��������B���������A���������ɑ��݂���قȂ�v
						// ���l�𐳂����O���t�ɕ`�悷�邱�Ƃ͂ł��Ȃ��B
						break;
					}
				}
			}
		}
		return ret;
	}
}
