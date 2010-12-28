/**
 * Describes a request from provider manager - summary.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */


package util.xml;

import interf.xml.XMLElem;

public class ProviderManagerRequestSum extends Event {

	public ProviderManagerRequestSum(String id) throws Exception {
		super(id, "pman_requested_prov_summary");
		// TODO Auto-generated constructor stub
	}

	/* allow addition of elements */
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}
}
