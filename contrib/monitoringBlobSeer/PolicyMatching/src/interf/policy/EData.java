/**
 * Class that represents the external necessary data of a security policy.
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
import util.xml.EModule;
import interf.list.IList;
import interf.list.IListFactory;
import interf.xml.XMLElem;

public final class EData implements Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = -6887574131334566544L;

	/** factory object for creating lists */
	protected final IListFactory<EModule> f = new ListFactory<EModule>();
	
	/** preconditions list, contains elements of type Module */
	IList<EModule> edata;
	
	/**
	 * Constructor
	 */
	public EData() {
		edata = f.makeEmpty();
	}
	
	/**
	 * Adds an extern data source, defined by the module provided as parameter.
	 * @param ids - adds element ids to this list
	 * @param elem
	 */
	public void addModule(HashSet<String> ids, EModule elem) {
		edata = f.makeNonEmpty(elem, edata);
		ids.addAll(elem.getIds());
	}
	
	/**
	 * Builds the XML for external data
	 * @param root - parent XML element
	 * @param document - XML document
	 * @param XMLalg - the algorithm for XML transformation
	 */
	public void buildXML(Element root, Document dom, XMLWriter alg) {
		// create element for preconditions section
		Element precondElem = dom.createElement("data");
		//convert the list to XML
		edata.toXML(alg, precondElem, dom);
		// add the newly created element
		root.appendChild(precondElem);

	}

}
