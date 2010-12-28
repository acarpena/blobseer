/**
 * Refers an event by its id.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class RefEvent extends SimpleXMLElem {

	public RefEvent(String id, String value)
			throws Exception {
		super("refEvent", id, value, null);
	}

}
