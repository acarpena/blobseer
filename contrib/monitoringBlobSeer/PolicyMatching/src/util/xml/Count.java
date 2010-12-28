/**
 * Count field.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class Count extends SimpleXMLElem {

	public Count(String id, String value, String rvalue)
		throws Exception {
		super("count", id, value, rvalue);
	}
	
	/**
	 * generic count
	 * @param what
	 * @param id
	 * @param value
	 * @param rvalue
	 * @throws Exception
	 */
	public Count(String what, String id, String value, String rvalue)
			throws Exception {
		super(what+"Count", id, value, rvalue);
		if(what == null)
			throw new Exception("Field 'what' cannot be null in count.");
	}

}
