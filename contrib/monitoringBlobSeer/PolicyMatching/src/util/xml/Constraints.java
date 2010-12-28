/**
 * Definition for constraints.
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

public class Constraints extends ComplexXMLElem {

	public Constraints(String id, String type) throws Exception {
		super("constraints", id, type);
	}
	
	/**
	 * Adition of and element
	 * @param elem
	 */
	public void addAnd(And elem) {
		super.addElem(elem);
	}
	
	/**
	 * Adition of or element
	 * @param elem
	 */
	public void addOr(Or elem) {
		super.addElem(elem);
	}
	
	/**
	 * Adition of neg element
	 * @param elem
	 */
	public void addNeg(Neg elem) {
		super.addElem(elem);
	}
	
	/**
	 * Adition of constraint element
	 * @param elem
	 */
	public void addConstraint(Content elem) {
		super.addElem(elem);
	}
	
}
