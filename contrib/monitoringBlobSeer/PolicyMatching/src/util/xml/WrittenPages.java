/**
 * Blob ID.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class WrittenPages extends SimpleXMLElem {

	public WrittenPages(String id, String value)
			throws Exception {
		super("written_pages", id, value, null);
	}

}
