/**
 * Class that represents the preconditions of a security policy.
 * 
 * @author Cristina Basescu
 * @date 24.03.2010
 */

package interf.policy;

import java.io.Serializable;
import java.util.HashSet;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import util.alg.XMLWriter;
import util.list.ListFactory;
import interf.list.IList;
import interf.list.IListFactory;
import interf.xml.XMLElem;

public final class Precond implements Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 8186879357506263154L;

	/** factory object for creating lists */
	protected final IListFactory<XMLElem> f = new ListFactory<XMLElem>();
	
	/** preconditions list, contains elements of type XMLElem */
	IList<XMLElem> precond;
	
	/**
	 * Constructor
	 */
	public Precond() {
		precond = f.makeEmpty();
	}
	
	/**
	 * Adds an action, defined by the XML element provided as parameter.
	 * @param ids - adds element ids to this list
	 * @param elem - action as an XMLElem
	 */
	public void addPrecond(HashSet<String> ids, XMLElem elem) {
		precond = f.makeNonEmpty(elem, precond);
		ids.addAll(elem.getIds());
	}
	
	/**
	 * Builds the XML for preconditions
	 * @param root - parent XML element
	 * @param document - XML document
	 * @param XMLalg - the algorithm for XML transformation
	 */
	public void buildXML(Element root, Document dom, XMLWriter alg) {
		// create element for preconditions section
		Element precondElem = dom.createElement("preconditions");
		//convert the list to XML
		precond.toXML(alg, precondElem, dom);
		// add the newly created element
		root.appendChild(precondElem);

	}
	
	public IList<XMLElem> getPrecond() {
		return precond;
	}

}
