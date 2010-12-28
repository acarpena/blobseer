/**
 * Describes a write to a data provider.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */


package util.xml;

import interf.xml.XMLElem;

public class Write extends Event {

	public Write(String id) throws Exception {
		super(id, "prov_write");
		// TODO Auto-generated constructor stub
	}

	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}
}
