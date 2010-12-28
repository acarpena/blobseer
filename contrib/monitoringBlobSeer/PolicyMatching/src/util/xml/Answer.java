/**
 * Definition for answer part of a module.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import interf.list.IListAlg;
import interf.xml.ComplexXMLElem;
import interf.xml.XMLElem;

public class Answer extends ComplexXMLElem {

	public Answer(String id, String type) throws Exception {
		super("answer", id, type);
	}
	
	public void addElem(XMLElem elem) {
		super.addElem(elem);
	}
}
