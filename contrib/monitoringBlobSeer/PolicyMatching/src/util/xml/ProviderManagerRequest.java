/**
 * Describes a request from provider manager.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */


package util.xml;

import interf.xml.XMLElem;

public class ProviderManagerRequest extends Event {

	public ProviderManagerRequest(String id) throws Exception {
		super(id, "pman_requested_prov");
		// TODO Auto-generated constructor stub
	}

	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}
}
