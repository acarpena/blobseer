/**
 * Describes a page request from data providers.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */


package util.xml;

import interf.xml.XMLElem;

public class PageRequest extends Event {

	public PageRequest(String id) throws Exception {
		super(id, "prov_read");
		// TODO Auto-generated constructor stub
	}

	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}
}
