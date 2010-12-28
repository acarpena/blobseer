/**
 * Page id.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class PageID extends SimpleXMLElem {

	public PageID(String id, String value, String rvalue)
			throws Exception {
		super("pageID", id, value, rvalue);
	}

}
