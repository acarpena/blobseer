/**
 * Definition for advices.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import java.util.LinkedList;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import interf.list.IListAlg;
import interf.xml.ComplexXMLElem;
import interf.xml.XMLElem;

public class Advices extends ComplexXMLElem {

	public Advices(String id, String type) throws Exception {
		super("advices", id, type);
	}
	
	/**
	 * Adition of and element
	 * @param elem
	 */
	public void addAnd(And elem) {
		super.addElem(elem);
	}
	
	/**
	 * Addition of or element
	 * @param elem
	 */
	public void addOr(Or elem) {
		super.addElem(elem);
	}
	
	/**
	 * Addition of advice.
	 * @param elem
	 */
	public void addAdvice(Advice elem) {
		super.addElem(elem);
	}
	
}
