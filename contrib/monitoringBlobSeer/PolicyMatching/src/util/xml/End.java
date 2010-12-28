/**
 * End event.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class End extends SimpleXMLElem {

	public End(String id, String value)
			throws Exception {
		super("end", id, value, null);
	}

}
