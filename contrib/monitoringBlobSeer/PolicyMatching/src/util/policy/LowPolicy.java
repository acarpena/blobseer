/**
 * Class defining a low priority policy. It can be instantiated, but not extended further.
 * Constrains on high priority policies:
 * - are not applicable by default
 * - can have as actions logging and decrease of trust level (trust level module decides how much, 
 *   depending on the type of policy that issued the action)
 * 
 * @author Cristina Basescu
 * @date 03.2010
 */
package util.policy;

import java.io.Serializable;
import java.util.Date;

import util.conf.Conf;
import util.xml.Action;
import util.xml.Advice;
import util.xml.Advices;
import util.xml.And;
import util.xml.DecreaseTL;
import util.xml.EnablePolicy;
import util.xml.Log;
import util.xml.Or;
import interf.policy.BasicPolicy;
import interf.policy.PolicyType;
import interf.xml.SimpleXMLElem;
import interf.xml.XMLElem;


public final class LowPolicy extends BasicPolicy{
	
	public LowPolicy(String clientId) throws Exception {
		super(clientId, Boolean.FALSE, PolicyType.LOW_POLICY);
		
		// set and add id
		id = PolicyType.LOW_POLICY + "_"+Conf.lastPolicyNum;
		Conf.incPolicyNum();
		
		frequence = 50;
	}
	
	public void addAction(Action act) throws Exception {
		// get pieces of advice for this rule
		Advices advices = act.getAdvices();
		// suppose we should add the rule
		Boolean ok = Boolean.TRUE;
		// get each advice and see if they are all accepted
		for(XMLElem elem : advices.getElems()) {
			// allows and, or and advice of the following types: logging, decrease trust level
			if((elem instanceof Log) || (elem instanceof DecreaseTL)) {
				ok = ok && Boolean.TRUE;
			}	
			else {
				if(!(elem instanceof And) && !(elem instanceof Or)) {
					ok = Boolean.FALSE;
					throw new Exception("Illegal action for this type of policy.");
				}
			}
		}
		if(ok)
			super.addAction(act);
	}
	
	public Integer getPolicyType() {
		return PolicyType.LOW_POLICY;
	}

}
