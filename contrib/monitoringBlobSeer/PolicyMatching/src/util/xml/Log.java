/**
 * Specifies an advice as logging.
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.xml;

public class Log extends Advice {

	/**
	 * 
	 * @param id
	 * @param value - string to be logged
	 * @throws Exception
	 */
	public Log(String id, String value) throws Exception {
		
		super("log", id, value, null);
	}
}