/**
 * Describes a request from metadata providers.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */


package util.xml;

import interf.xml.XMLElem;

public class MetadataPageRequest extends Event {

	public MetadataPageRequest(String id) throws Exception {
		super(id, "mprov_page_id_request");
		// TODO Auto-generated constructor stub
	}

	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}
}
