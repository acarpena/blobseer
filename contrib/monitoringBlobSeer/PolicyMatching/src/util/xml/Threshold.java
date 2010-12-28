/**
 * Threshold field.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class Threshold extends SimpleXMLElem {

	public Threshold(String id, String value)
		throws Exception {
		super("threshold", id, value, null);
	}
	
	/**
	 * generic threshold
	 * @param what
	 * @param id
	 * @param value
	 * @param rvalue
	 * @throws Exception
	 */
	public Threshold(String what, String id, String value)
			throws Exception {
		super("threshold"+what, id, value, null);
		if(what == null)
			throw new Exception("Field 'what' cannot be null in count.");
	}

}
