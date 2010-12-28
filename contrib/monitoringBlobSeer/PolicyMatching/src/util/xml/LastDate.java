/**
 * Last date.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class LastDate extends SimpleXMLElem {

	public LastDate(String id, String value, String rvalue)
			throws Exception {
		super("lastDate", id, value, rvalue);
	}

}
