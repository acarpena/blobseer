/**
 * Generic non-empty list implementation (for the recursively defined list).
 * Used to memorize a XML structure.
 * 
 * @author Cristina Basescu
 * @date 24.03.2010
 */

package interf.list;

public interface NEIList<E> extends IList<E>{
	/**
	 * Gets the first element in the list.
	 * @return the first element
	 */
	public E getFirst();
	
	/**
	 * Gets the tail of the list
	 * @return the tail
	 */
	public IList<? extends E> getRest();
}

