/**
 * Describes a request from metadata providers - summary.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */


package util.xml;

import interf.xml.XMLElem;

public class MetadataNodeRequestSum extends Event {

	public MetadataNodeRequestSum(String id) throws Exception {
		super(id, "mprov_node_requests_summary");
		// TODO Auto-generated constructor stub
	}

	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}
}
