/**
 * Algorithm for converting a list of XMLElem into XML format.
 * 
 * @author Cristina Basescu
 * @date 24.03.2010
 */

package util.alg;

import java.io.Serializable;

import org.w3c.dom.Document;
import org.w3c.dom.Element;


import interf.list.EIList;
import interf.list.IListAlg;
import interf.list.NEIList;
import interf.xml.XMLElem;

public class XMLWriter implements IListAlg<XMLElem>, Serializable {

	/**
	 * 
	 */
	private static final long serialVersionUID = -2636918228209053711L;

	public void emptyCase(EIList<? extends XMLElem> list, Element root,
			Document dom) {
		// don't write anything
	}

	public void nonEmptyCase(NEIList<? extends XMLElem> list, Element root,
			Document dom) {
		// process separately the first element and the tail of the list
		XMLElem first = list.getFirst();
		// the elements are in reversed order in the list, so process the tail before the first element
		list.getRest().toXML(this, root, dom);
		if(first != null)
			try {
				first.processXML(this, root, dom);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	}
}
