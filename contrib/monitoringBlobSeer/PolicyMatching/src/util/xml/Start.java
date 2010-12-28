/**
 * Start event.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class Start extends SimpleXMLElem {

	public Start(String id, String value)
			throws Exception {
		super("start", id, value, null);
	}

}
