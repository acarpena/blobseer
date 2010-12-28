/**
 * Specifies an advice as decrease of trust level.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

public class DecreaseTL extends Advice {

	/**
	 * Advice sent directly to Trust Level module
	 * @param id
	 * @param hint - how much to decrease the trust level, e.g. 30, medium
	 * @throws Exception
	 */
	public DecreaseTL(String id, String hint) throws Exception {
		
		super("decreaseTL", id, hint, null);
	}
}