/**
 * Describes a request from metadata providers.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */


package util.xml;

import interf.xml.XMLElem;

public class MetadataNodeRequest extends Event {

	public MetadataNodeRequest(String id) throws Exception {
		super(id, "mprov_node_request");
		// TODO Auto-generated constructor stub
	}

	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}
}
