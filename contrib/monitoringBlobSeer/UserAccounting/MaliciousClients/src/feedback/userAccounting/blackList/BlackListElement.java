package feedback.userAccounting.blackList;

/**
 * @author acarpena
 *
 * Generic class that encapsulates an element of the black list
 */
public class BlackListElement implements Comparable<BlackListElement>{
	
	// the field that needs to be contained in any type of list element is the ID of the client
	protected String clientID;
	
	public BlackListElement( String s)
	{
		clientID = s;
	}
	
	
	public String getClientID() {
		return clientID;
	}

	public void setClientID(String clientID) {
		this.clientID = clientID;
	}

	// the compareTo method is needed in case the list needs to be sorted
	public int compareTo(BlackListElement el)
	{
		return clientID.compareTo(el.getClientID());
	}
}
