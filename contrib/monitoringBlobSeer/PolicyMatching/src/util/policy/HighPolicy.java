/**
 * Class defining a high priority policy. It can be instantiated, but not extended further.
 * Constrains on high priority policies:
 * - are applied always
 * - can have as actions logging, enable of low priority policy, blacklist a user, decrease trust level
 * 
 * @author Cristina Basescu
 * @date 03.2010
 */
package util.policy;

import java.io.Serializable;

import util.conf.Conf;
import util.xml.Action;
import util.xml.Advice;
import util.xml.Advices;
import util.xml.And;
import util.xml.EnablePolicy;
import util.xml.Or;

import interf.policy.BasicPolicy;
import interf.policy.PolicyType;
import interf.xml.XMLElem;


public final class HighPolicy extends BasicPolicy{
	
	public HighPolicy(String clientId) throws Exception {
		super(clientId, Boolean.TRUE, PolicyType.HIGH_POLICY);
		
		// set and add id
		id = PolicyType.HIGH_POLICY + "_"+Conf.lastPolicyNum;
		Conf.incPolicyNum();
		
		// set available policy types for enable
		this.allowEnablePolicy(PolicyType.LOW_POLICY);
		
		frequence = 2;
	}
	
	public void addAction(Action act) throws Exception {
		// get pieces of advice for this rule
		Advices advices = act.getAdvices();
		// suppose we should add the rule
		Boolean ok = Boolean.TRUE;
		// get each advice and see if they are all accepted
		for(XMLElem elem : advices.getElems()) {
			// allows and, or and advice
			// enables only low level policies
			if(elem instanceof EnablePolicy) {
				// add enable policy only if permitted
				if(enabledPolicies.contains(((EnablePolicy)elem).getPolicyType())) {
					ok = ok && Boolean.TRUE;
				}
				else if(!(elem instanceof And) && !(elem instanceof Or) && !(elem instanceof Advice)) {
						ok = Boolean.FALSE;
						throw new Exception("Cannot enable the desired policy.");
				}
			}
		}
		if(ok)
			super.addAction(act);
	}
	
	
	public Integer getPolicyType() {
		return PolicyType.HIGH_POLICY;
	}
}
