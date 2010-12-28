/**
 * Or logic relation.
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

public class Or extends ComplexXMLElem {
	
	public Or(String id) throws Exception {
		super("or", null, null);
	}
	
	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}

}
