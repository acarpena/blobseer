/**
 * Metadata provider.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class MetadataProvider extends SimpleXMLElem {

	public MetadataProvider(String id, String value, String rvalue)
			throws Exception {
		super("metadataProvider", id, value, rvalue);
	}

}
