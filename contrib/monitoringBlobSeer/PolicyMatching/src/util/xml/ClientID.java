/**
 * Client id.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class ClientID extends SimpleXMLElem {

	public ClientID(String id, String value, String lvalue)
			throws Exception {
		super("clientID", id, value,lvalue);
	}

}
