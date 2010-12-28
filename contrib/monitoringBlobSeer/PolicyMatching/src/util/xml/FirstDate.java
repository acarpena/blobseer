/**
 * First date.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class FirstDate extends SimpleXMLElem {

	public FirstDate(String id, String value, String rvalue)
			throws Exception {
		super("firstDate", id, value, rvalue);
	}

}
