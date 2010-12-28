/**
 * Describes a write to a data provider - summary.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */


package util.xml;

import interf.xml.XMLElem;

public class WriteSum extends Event {

	public WriteSum(String id) throws Exception {
		super(id, "prov_write_summary");
		// TODO Auto-generated constructor stub
	}

	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}
}
