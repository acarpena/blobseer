/**
 * Class defining a medium priority policy. It can be instantiated, but not extended further.
 * Constrains on high priority policies:
 * - are applied from time to time
 * - can have as actions logging, enable of low priority policy, decrease trust level
 * 
 * @author Cristina Basescu
 * @date 03.2010
 */
package util.policy;

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
import interf.xml.XMLElem;


public final class MediumPolicy extends BasicPolicy {
	
	public MediumPolicy(String clientId) throws Exception {
		super(clientId, Boolean.TRUE, PolicyType.MEDIUM_POLICY);
		
		// set and add id
		id = PolicyType.MEDIUM_POLICY + "_"+Conf.lastPolicyNum;
		Conf.incPolicyNum();
		
		// set available policy types for enable
		this.allowEnablePolicy(PolicyType.LOW_POLICY);
		
		frequence = 20;
	}
	
	public void addAction(Action act) throws Exception {
		// get pieces of advice for this rule
		Advices advices = act.getAdvices();
		// suppose we should add the rule
		Boolean ok = Boolean.TRUE;
		// get each advice and see if they are all accepted
		for(XMLElem elem : advices.getElems()) {
			// allows and, or and advice of the following types: logging, decrease trust level,enable policy
			if((elem instanceof Log) || (elem instanceof DecreaseTL)) {
				ok = ok && Boolean.TRUE;
			}
			else {
				// enables only low level policies
				if(elem instanceof EnablePolicy) {
					if(enabledPolicies.contains(((EnablePolicy)elem).getPolicyType())) {
						ok = ok && Boolean.TRUE;
					}
					else {
						ok = Boolean.FALSE;
						throw new Exception("Cannot enable the desired policy.");
					}
				}
				else {
					if(!(elem instanceof And) && !(elem instanceof Or)) {
						ok = Boolean.FALSE;
						throw new Exception("Illegal action for this type of policy.");
					}
				}
			}
		}
		if(ok)
			super.addAction(act);
	}
	
	public Integer getPolicyType() {
		return PolicyType.MEDIUM_POLICY;
	}

}
