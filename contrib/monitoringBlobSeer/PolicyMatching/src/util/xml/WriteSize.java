/**
 * Write size.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class WriteSize extends SimpleXMLElem {

	public WriteSize(String id, String value, String rvalue)
			throws Exception {
		super("writeSize", id, value, rvalue);
	}

}
