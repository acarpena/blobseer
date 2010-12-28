/**
 * Class containing a complex XML element, with children and attributes.
 * 
 * @author Cristina Basescu
 * @date 03.2010
 */

package interf.xml;

import java.io.Serializable;
import java.util.HashSet;
import java.util.LinkedList;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import util.list.ListFactory;

import interf.list.IList;
import interf.list.IListAlg;
import interf.list.IListFactory;
import interf.list.NEIList;

public class ComplexXMLElem extends XMLElem implements Serializable {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 6611476126670583592L;

	/** element type */
	public String type;
	
	/** list of children */
	protected IList<XMLElem> value;
	
	/** factory for creating lists; private because one can add children just using
	 * addElem method */
	private final IListFactory<XMLElem> f = new ListFactory<XMLElem>();
	
	public ComplexXMLElem(String name, String id, String type) throws Exception {
		super(name, id);
		this.type = type;
		this.value = f.makeEmpty();
	}
	
	/**
	 * adds a child
	 * @param elem
	 */
	protected void addElem(XMLElem elem) {
		value = f.makeNonEmpty(elem, value);
		ids.addAll(elem.getIds());
	}
	
	@Override
	public void processXML(IListAlg<XMLElem> alg, Element root, Document dom) throws Exception {
		// create element
		Element thisElem = dom.createElement(this.name);
		
		// set attributed
		if(this.id != null)
			thisElem.setAttribute("id", this.id);
		if(this.type != null)
			thisElem.setAttribute("type", this.type);
		
		// recursively create children
		value.toXML(alg, thisElem, dom);
		root.appendChild(thisElem);
	}
	
	/**
	 * @return children of current element in a LinkedList
	 */
	public LinkedList<XMLElem> getElems() {
		IList<XMLElem> aux = value;
		LinkedList<XMLElem> ret = new LinkedList<XMLElem>();
		while(aux instanceof NEIList<?>) {
			XMLElem crt = ((NEIList<XMLElem>)aux).getFirst();
			ret.add(crt);
			if(crt instanceof ComplexXMLElem) {
				ret.addAll(((ComplexXMLElem)crt).getElems());
			}
			aux = (IList<XMLElem>)((NEIList<XMLElem>)aux).getRest();
		}
		return ret;
	}
}