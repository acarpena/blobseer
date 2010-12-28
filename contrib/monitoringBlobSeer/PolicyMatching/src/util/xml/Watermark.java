/**
 * Watermark.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import interf.xml.SimpleXMLElem;

public class Watermark extends SimpleXMLElem {

	public Watermark(String id, String value, String rvalue)
			throws Exception {
		super("watermark", id, value, rvalue);
	}

}
