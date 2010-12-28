/**
 * Content field - contains operation.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import util.policy.Operation;
import interf.xml.SimpleXMLElem;

public class Content extends SimpleXMLElem {

	public Content(String id, Operation value)
			throws Exception {
		super("content", id, value.toString(), null);
	}

}
