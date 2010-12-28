/**
 * Specifies an advice as blacklist of a user.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

public class Blacklist extends Advice {

	/**
	 * 
	 * @param id
	 * @param value - fixed client id to blacklist
	 * @param lvalue - variable client id to blacklist
	 * @throws Exception
	 */
	public Blacklist(String id, String value, String lvalue) throws Exception {
		
		super("blacklist", id, value, lvalue);
	}
}