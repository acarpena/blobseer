/**
 * Class containing a simple XML element, that has no children, but only attributes.
 * 
 * @author Cristina Basescu
 * @date 03.2010
 */

package interf.xml;

import java.io.Serializable;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Text;

import interf.list.IListAlg;

public class SimpleXMLElem extends XMLElem implements Serializable {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 3766254924249712638L;

	/** element fixed value */
	public String value;
	
	/** element modifiable value */
	public String rvalue;
	
	public SimpleXMLElem(String name, String id, String value, String rvalue) throws Exception {
		super(name, id);
		this.value = value;
		this.rvalue = rvalue;
	}

	@Override
	public void processXML(IListAlg<XMLElem> alg, Element root, Document dom) {
		// create element
		Element thisElem = dom.createElement(this.name);
		
		// set attributes
		if(this.id != null)
			thisElem.setAttribute("id", this.id);
		if(this.value != null)
			thisElem.setAttribute("value", this.value);
		if(this.rvalue != null)
			thisElem.setAttribute("rvalue", this.rvalue);
		
		// append current element to root element
		root.appendChild(thisElem);
	}
}
