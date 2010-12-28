/**
 * Describes a page request from data providers.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */


package util.xml;

import interf.xml.XMLElem;

public class Check extends Event {

	public Check(String id) throws Exception {
		super(id, "check");
		// TODO Auto-generated constructor stub
	}

	/* allow addition of content and logical ops */
	public void addElem(Content elem) {
		super.addElem(elem);
	}
	
	public void addElem(Or elem) {
		super.addElem(elem);
	}
	
	public void addElem(And elem) {
		super.addElem(elem);
	}
	
	public void addElem(Neg elem) {
		super.addElem(elem);
	}
}
