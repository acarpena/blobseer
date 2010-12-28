/**
 * Definition for an action.
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

/*
 * Must contain exactly one of each: constraints and advices elements.
 */
public class Action extends ComplexXMLElem {

	private Boolean constraintsSet;
	private Boolean advicesSet;
	
	private Advices advices;
	
	public Action(String id) throws Exception {
		super("action", id, null);
		constraintsSet = Boolean.FALSE;
		advicesSet = Boolean.FALSE;
		advices = null;
	}

	public void setConstraints(Constraints q) throws Exception {
		if(!constraintsSet) {
			constraintsSet = Boolean.TRUE;
			super.addElem(q);
		}
		else throw new Exception("Constraints already set for module.");
	}
	
	public void setAdvices(Advices a) throws Exception {
		if(!advicesSet) {
			advicesSet = Boolean.TRUE;
			super.addElem(a);
			this.advices = a;
		}
		else throw new Exception("Constraints already set for module.");
	}
	
	@Override
	public void processXML(IListAlg<XMLElem> alg, Element root, Document dom) throws Exception{
		if(!constraintsSet || !advicesSet)
			throw new Exception("Constraints or Advices not set in Action element.");
			super.processXML(alg, root, dom);
	}
	
	public Advices getAdvices() {
		return advices;
	}
	
}
