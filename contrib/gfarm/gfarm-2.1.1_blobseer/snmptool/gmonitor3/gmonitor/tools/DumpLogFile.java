/*
 * Created on 2003/06/27
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package gmonitor.tools;

import gmonitor.logdata.FirstMetaBlock;
import gmonitor.logdata.SecondMetaBlock;
import gmonitor.logdata.UTY;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

/**
 * @author hkondo
 *
 * ���O�t�@�C���̍\����ǂݎ��Aprintable �ȃe�L�X�g�`���Ń_���v����J���p�c�[��
 */
public class DumpLogFile {

	protected boolean swFMBOutput = true;
	protected boolean swSMBOutput = true;
	protected boolean swDBOoutput = true;
	protected String filename = "glogger.bin";

	public DumpLogFile()
	{
	}
	public DumpLogFile(String args[])
	{
		for(int i = 0; i < args.length; i++){
			if(args[i].startsWith("-")){
				// �I�v�V�����̏���
			}else{
				// �t�@�C�����̏���
				filename = args[i];
			}
		}
	}

	public void run()
	{
		FirstMetaBlock fmb;
		SecondMetaBlock smb;
		try {
			File f = new File(filename);
			FileInputStream fis = new FileInputStream(f);
			/* FirstMetaBlock */{
				byte[] bytes = new byte[2];
				fis.read(bytes);
				int sz = UTY.byte2int(bytes); 
				fmb = FirstMetaBlock.newInstance(fis, sz);
			}
						
			/* SecondMetaBlock */{
				byte[] bytes = new byte[4];
				fis.read(bytes);
				int sz = UTY.byte2int(bytes);
				smb = SecondMetaBlock.newInstance(fis, sz);
			}

			if(swFMBOutput == true){
				System.out.println(fmb.asDump());
			}
			if(swSMBOutput == true){
				System.out.println(smb.asDump());
			}
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void main(String args[])
	{
		DumpLogFile dlf = new DumpLogFile(args);
		dlf.run();
	}
}
