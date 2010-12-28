/**
 * Element representing the validity of current policy.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class Active extends SimpleXMLElem {

	public Active(String id, String value)
			throws Exception {
		super("active", id, value, null);
		if(value.compareTo("true") != 0 && value.compareTo("false") != 0)
			throw new Exception("Active must be either true or false.");
	}

}
