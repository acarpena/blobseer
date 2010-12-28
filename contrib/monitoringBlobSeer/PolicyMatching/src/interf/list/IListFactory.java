/**
 * Factory design patternGeneric non-empty list implementation (for the recursively defined list).
 * Used to memorize a XML structure.
 * 
 * @author Cristina Basescu
 * @date 24.03.2010
 */


package interf.list;


public interface IListFactory<E> {
	
	/**
	 * Build an empty list.
	 * @return the newly created list
	 */
	public EIList<E> makeEmpty();
	
	/**
	 * Builds a non-empty list.
	 * @param first - first element in the list
	 * @param rest - the tail, consisting of another list
	 * @return the newly created list
	 */
	public NEIList<E> makeNonEmpty(E first, IList<? extends E> rest);
}

