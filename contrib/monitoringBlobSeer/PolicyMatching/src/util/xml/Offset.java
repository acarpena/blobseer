/**
 * Offset in a write operation.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class Offset extends SimpleXMLElem {

	public Offset(String id, String value, String rvalue)
			throws Exception {
		super("offset", id, value, rvalue);
	}

}
