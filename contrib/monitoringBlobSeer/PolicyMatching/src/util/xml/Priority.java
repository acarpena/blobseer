/**
 * Policy priority.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class Priority extends SimpleXMLElem {

	public Priority(String id, String value)
			throws Exception {
		super("priority", id, value, null);
	}

}
