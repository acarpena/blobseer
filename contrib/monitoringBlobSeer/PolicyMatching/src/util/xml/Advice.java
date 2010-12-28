/**
 * Specifies and advice.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public abstract class Advice extends SimpleXMLElem {

	public Advice(String name, String id, String value, String lvalue) throws Exception {
		super(name, id, value, lvalue);
	}
}