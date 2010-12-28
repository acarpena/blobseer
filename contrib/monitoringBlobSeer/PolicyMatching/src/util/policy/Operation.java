/**
 * Basic operation that can be found in policies.
 * Can be either 'operand1 op operand2' or 'op operand2' 
 * 
 * @author Cristina Basescu
 * @date 04.2010
 */

package util.policy;

import java.util.HashSet;

import util.conf.Conf;

public class Operation {

	public String operand1;
	public String operand2;
	public int op;
	
	// valid operations
	public static int PLUS = 0;
	public static int MINUS = 1;
	public static int GTE = 2;
	public static int LTE = 3;
	public static int GT = 4;
	public static int LT = 5;
	public static int NEQ = 6;
	public static int EQ = 7;
	public static int MULTIPLY = 8;
	public static int DIVISION = 9;
	public static int SUBSET = 10;
	
	/**
	 * Constructor
	 * @param elementIds - operands must represent ids from this set (if they are not numbers or
	 * 		Operation objects)
	 * @param operand1 - first operand
	 * @param operand2 - second operand
	 * @param op - operation
	 * @param allowedIds - operands can also be from this set
	 * @throws Exception
	 */
	public Operation(HashSet<String> elementIds, Object operand1, Object operand2, int op,
			String... ids ) throws Exception {
		// build allowed ids set
		HashSet<String> allowedIds = new HashSet<String>();
		if(elementIds != null)
			allowedIds = new HashSet<String>(elementIds);
		for(String s: ids)
			allowedIds.add(s);
		
		// allow just second operand, for things like '>= x'
		if(operand2 == null)
			throw new Exception("operand2 cannot be null.");
		
		// check whether operand1 and operand2 should be a valid id
		if(operand2 instanceof String)
			if(allowedIds.contains(operand2) == false)
				throw new Exception("operand2 " + operand2 + " is an invalid id for the current policy.");
		
		if(operand1 instanceof String)
			if(allowedIds.contains(operand1) == false)
				throw new Exception("operand1 " + operand1 + " is an invalid id for the current policy.");
		
		if(op < 0 || op > 10)
			throw new Exception("Invalid operation.");
		
		if (operand1 == null && (op == Operation.PLUS || op == Operation.MINUS || 
				op == Operation.MULTIPLY || op == Operation.DIVISION) )
			throw new Exception("Cannot have less than two operands for a 2-operand-operation.");
		
		if(operand1 != null)
			this.operand1 = operand1.toString();
		this.operand2 = operand2.toString();
		this.op = op;
	}
	
	public String toString() {
		String sOp = "";
		switch(op) {
		case 0: sOp="+"; Conf.opConstants += "+"; break;
		case 1: sOp="-"; Conf.opConstants += "-"; break;
		case 2: sOp=">="; Conf.opConstants += ">="; break;
		case 3: sOp="<="; Conf.opConstants += "<="; break;
		case 4: sOp=">"; Conf.opConstants += ">"; break;
		case 5: sOp="<"; Conf.opConstants += "<"; break;
		case 6: sOp="!="; Conf.opConstants += "!="; break;
		case 7: sOp="="; Conf.opConstants += "="; break;
		case 8: sOp="*"; Conf.opConstants += "*"; break;
		case 9: sOp="/"; Conf.opConstants += "/"; break;
		case 10: sOp="⊂"; Conf.opConstants += "⊂"; break;
		}
		
		if(operand1 == null)
			return sOp + " " + operand2;
		else
			return operand1 + " " + sOp + " " + operand2;
	}
}
