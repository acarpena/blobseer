/**
 * Specifies distance in time.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import util.policy.Operation;
import interf.xml.SimpleXMLElem;

public class Distance extends SimpleXMLElem {

	public Distance(String id, Operation o)
			throws Exception {
		super("distance", id, o.toString(), null);
	}

	public Distance(String id, Integer nr)
			throws Exception {
		super("distance", id, nr.toString(), null);
	}
	
	/** infinite distance */
	public Distance(String id) throws Exception {
		super("distance", id, "infinite", null);
	}
}
