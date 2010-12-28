/**
 * Negation logic relation.
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

public class Neg extends ComplexXMLElem {
	
	public Neg(String id) throws Exception {
		super("neg", id, null);
	}
	
	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}

}
