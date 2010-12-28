/**
 * Superior Threshold field.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class SupThreshold extends SimpleXMLElem {

	public SupThreshold(String id, String value)
		throws Exception {
		super("supThreshold", id, value, null);
	}
	
	/**
	 * generic threshold
	 * @param what
	 * @param id
	 * @param value
	 * @param rvalue
	 * @throws Exception
	 */
	public SupThreshold(String what, String id, String value)
			throws Exception {
		super("supThreshold"+what, id, value, null);
		if(what == null)
			throw new Exception("Field 'what' cannot be null in count.");
	}

}
