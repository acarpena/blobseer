/**
 * Provides the means for describing followed by relation.
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
 * Can contain maximum one of each: refEvent, count and distance elements.
 */
public class FollowedBy extends ComplexXMLElem {
	
	private Boolean refEventSet;
	private Boolean countSet;
	private Boolean distSet; 
	
	public FollowedBy(String id) throws Exception {
		super("followedBy", id, null);
		refEventSet = Boolean.FALSE;
		countSet = Boolean.FALSE;
		distSet = Boolean.FALSE;
	}

	public void addRefEvent(RefEvent re) throws Exception {
		if(!refEventSet) {
			refEventSet = Boolean.TRUE;
			super.addElem(re);
		}
		else throw new Exception("RefEvent already set for precededBy.");
	}
	
	public void addCount(Count c) throws Exception {
		if(!countSet) {
			countSet = Boolean.TRUE;
			super.addElem(c);
		}
		else throw new Exception("Count already set for precededBy.");
	}
	
	public void addDistance(Distance d) throws Exception {
		if(!distSet) {
			distSet = Boolean.TRUE;
			super.addElem(d);
		}
		else throw new Exception("Distance already set for precededBy.");
	}
}
