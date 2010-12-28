/**
 * And logic relation.
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

public class And extends ComplexXMLElem {
	
	public And(String id) throws Exception {
		super("and", id, null);
	}
	
	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}

}
