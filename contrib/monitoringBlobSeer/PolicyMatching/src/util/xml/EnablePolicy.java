/**
 * Specifies and advice as enabling a policy.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

import java.util.StringTokenizer;

public class EnablePolicy extends Advice {

	private String policy;
	
	/**
	 * 
	 * @param id
	 * @param policy - value id of the policy to enable of the form a_b
	 * @throws Exception
	 */
	public EnablePolicy(String id, String policy) throws Exception {
		
		super("enablePolicy", id, policy, null);
		
		if(!policy.contains("_"))
			throw new Exception("Wrong policy id in policyEnable.");
		
		this.policy = policy;
	}
	
	/**
	 * @return type of policy
	 */
	public Integer getPolicyType() {
		StringTokenizer st = new StringTokenizer(this.policy, "_");
		return new Integer(st.nextToken());
	}

}