/**
 * Generic algorithm for generic list implementation.
 * It is used for XML conversion, applied differently on empty and non-empty lists.
 * 
 * @author Cristina Basescu
 * @date 24.03.2010
 */

package interf.list;


import org.w3c.dom.Document;
import org.w3c.dom.Element;

public interface IListAlg <E> {
	
	/**
	 * Algorithm on empty list
	 * @param list - empty list
	 * @param root - parent XML element
	 * @param dom - XML document
	 */
	void emptyCase (EIList <? extends E> list, Element root, Document dom);
	
	/**
	 * Algorithm on non-empty list
	 * @param list - contains elements to be converted into XML representation
	 * @param root - parent XML element
	 * @param dom - XML document
	 */
	void nonEmptyCase (NEIList <? extends E> list, Element root, Document dom);
}
