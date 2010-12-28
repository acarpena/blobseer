/**
 * Blob ID.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class BlobID extends SimpleXMLElem {

	public BlobID(String id, String value, String rvalue)
			throws Exception {
		super("blobId", id, value, rvalue);
	}

}
