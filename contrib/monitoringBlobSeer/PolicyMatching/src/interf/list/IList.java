/**
 * Generic list implementation, recursively defined.
 * Used to memorize a XML structure.
 * 
 * @author Cristina Basescu
 * @date 24.03.2010
 */

package interf.list;


import org.w3c.dom.Document;
import org.w3c.dom.Element;

public interface IList <E> {
	
	/**
	 * Writes the contents of the list as a child of XML element root, in the XML document dom
	 * @param alg - the algorithm applied on the list that converts to XML format
	 * @param root - parent XML element
	 * @param dom - XML document
	 */
	void toXML(IListAlg <? super E> alg, Element root, Document dom);
}
