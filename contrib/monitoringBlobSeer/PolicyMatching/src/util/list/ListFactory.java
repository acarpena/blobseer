/**
 * Factory class for lists (factory design pattern).
 * 
 * @author Cristina Basescu
 * @date 03.2010
 */


package util.list;

import java.io.Serializable;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import interf.list.EIList;
import interf.list.IList;
import interf.list.IListAlg;
import interf.list.IListFactory;
import interf.list.NEIList;


public class ListFactory<E> implements IListFactory<E>, Serializable {

	/**
	 * 
	 */
	private static final long serialVersionUID = -4432738583446628357L;
	
	private final EIList<E> empty = new EList<E>();
	
	/**
	 * Make empty list.
	 */
	public EIList<E> makeEmpty() {
		return empty;
	}
	
	/**
	 * Make non-empty list.
	 */
	public NEIList<E> makeNonEmpty(final E first, final IList<? extends E> rest) {
		return new NEList<E>(first, rest);
	}
	
	private class EList<E> implements EIList<E>, Serializable {
		/**
		 * 
		 */
		private static final long serialVersionUID = 8699335367124713641L;

		public void toXML(IListAlg<? super E> alg, Element root, Document dom) {
			// for an empty list, apply the algorithm for empty lists
			alg.emptyCase(this, root, dom);
		}
	}
	
	private class NEList<E> implements NEIList<E>, Serializable {
		/**
		 * 
		 */
		private static final long serialVersionUID = 4002039844735033525L;
		E first;
		IList<? extends E> rest;
		
		NEList(E first, IList<? extends E> rest) {
			this.first = first;
			this.rest = rest;
		}
		
		public E getFirst() {
			return first;
		}
		
		public IList<? extends E> getRest() {
			return rest;
		}
		
		public void toXML(IListAlg<? super E> alg, Element root,
				Document dom) {
			// for a non-empty list, apply the algorithm for non-empty lists
			alg.nonEmptyCase(this, root, dom);
		}
	}
}

