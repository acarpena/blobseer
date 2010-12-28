/**
 * Class for an XML element as stored in the list.
 * 
 * @author Cristina Basescu
 * @date 24.03.2010
 */

package interf.xml;

import java.io.Serializable;
import java.util.HashSet;

import interf.list.IListAlg;
import interf.list.IListFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import util.list.ListFactory;

public abstract class XMLElem implements Serializable {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = -3526854369074027664L;

	/** element tag */
	public String name;
	
	/** element id */
	public String id;
	
	/** ids of all children, including self; package access, should not be modified otherwise */
	HashSet<String> ids;
	
	/**
	 * Constructor
	 * @param name
	 * @param id
	 * @throws Exception
	 */
	protected XMLElem(String name, String id) throws Exception {
		if(name == null)
			throw new Exception("Cannot define an element with no name.");
		
		this.name = name;
		this.id = id;
		ids = new HashSet<String>();
		ids.add(id);
	}
	
	public HashSet<String> getIds() {
		return ids;
	}
	
	/**
	 * Builds the XML for the current object
	 * @param alg - the algorithm for XML transformation
	 * @param root - parent XML element
	 * @param document - XML document
	 * @throws Exception 
	 */
	public abstract void processXML(IListAlg<XMLElem> alg, Element root, Document dom) throws Exception;
}
