/**
 * Class that represents the action part of a security policy.
 * 
 * @author Cristina Basescu
 * @date 03.2010
 */

package interf.policy;

import java.io.Serializable;
import java.util.HashSet;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import util.alg.XMLWriter;
import util.list.ListFactory;
import util.xml.Action;
import interf.list.IList;
import interf.list.IListFactory;
import interf.xml.XMLElem;

public final class Actions implements Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 395827394700691147L;

	/** factory object for creating lists */
	protected final IListFactory<XMLElem> f = new ListFactory<XMLElem>();
	
	/** actions list, contains elements of type XMLElem */
	IList<XMLElem> actions;
	
	/**
	 * Constructor that initializes actions as an empty list.
	 */
	public Actions() {
		actions = f.makeEmpty();
	}
	
	/**
	 * Adds an action, defined by the XML element provided as parameter.
	 * @param ids - adds element ids to this list
	 * @param elem - action as an XMLElem
	 */
	public void addAction(HashSet<String> ids, Action elem) {
		actions = f.makeNonEmpty(elem, actions);
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
		Element actionElem = dom.createElement("actions");
		//convert the list to XML
		actions.toXML(alg, actionElem, dom);
		// add the newly created element
		root.appendChild(actionElem);

	}
	
}
