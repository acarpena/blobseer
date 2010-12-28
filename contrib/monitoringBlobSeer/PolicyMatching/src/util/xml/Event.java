/**
 * Describes an event, used in preconditions.
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

class Event extends ComplexXMLElem {
	
	public Event(String id, String type) throws Exception {
		super("event", id, type);
	}

	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}
	
}
