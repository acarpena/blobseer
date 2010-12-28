/**
 * Specifies a continuous matching for the referred fields.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import interf.list.IListAlg;
import interf.xml.ComplexXMLElem;
import interf.xml.XMLElem;

/*
 * Can contain maximum one of each: refEvent, count and distance elements.
 */
public class Continuous extends ComplexXMLElem {
	
	public Continuous(String id) throws Exception {
		super("continuous", id, null);
	}

	public void addRefEvent(RefEvent re) {
		super.addElem(re);
	}
	
}
