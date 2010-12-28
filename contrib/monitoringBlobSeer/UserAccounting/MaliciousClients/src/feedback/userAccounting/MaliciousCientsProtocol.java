package feedback.userAccounting;

import java.util.Vector;

import feedback.userAccounting.blackList.BasicBlackList;
import feedback.userAccounting.blackList.BlackListElement;

public class MaliciousCientsProtocol {
	
	private BasicBlackList blackList;
	
	public MaliciousCientsProtocol(BasicBlackList bl) {
		this.blackList = bl;
	}
	
	public Vector<String> processInput(String in) {
		
		Vector<String> output = new Vector<String>();
		
		// no request
		if (in == null) {
			return null;
		}
		
		// the BLACKLIST is requested
		if (in.equals("GET_BLACKLIST")) {
			// obtain the blacklist
			Vector<BlackListElement> list = blackList.getList();
			
			// the result is a vector containing the size of the list, 
			// followed by its elements (just the client ID for each element)
			output.add("" + list.size());
			for (BlackListElement elem:list) {
				output.add(elem.getClientID());
			}
		}
		
		// the end
		if (in.equals("END")) {
			output.add("END");
		}
		
		return output;
	}

}
