/**
 * Definition for a module representing an external data source.
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
 * Must contain exactly one of each: query and answer elements.
 */
public class EModule extends ComplexXMLElem {

	private Boolean querySet;
	private Boolean answerSet;
	
	public EModule(String id, String module) throws Exception {
		super("extern", id, module);
		querySet = Boolean.FALSE;
		answerSet = Boolean.FALSE;
	}

	public void setQuery(Query q) throws Exception {
		if(!querySet) {
			querySet = Boolean.TRUE;
			super.addElem(q);
		}
		else throw new Exception("Query already set for module.");
	}
	
	public void setAnswer(Answer a) throws Exception {
		if(!answerSet) {
			answerSet = Boolean.TRUE;
			super.addElem(a);
		}
		else throw new Exception("Query already set for module.");
	}
	
	@Override
	public void processXML(IListAlg<XMLElem> alg, Element root, Document dom) throws Exception{
		if(!querySet || !answerSet)
			throw new Exception("Query or answer not set in Module element.");
			super.processXML(alg, root, dom);
	}
	
}
