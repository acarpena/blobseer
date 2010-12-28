package fblobseer;

public class ReturnValues 
{
	private boolean result;
	private int errorCode;
	
	public ReturnValues() {
		
	}
	
	public ReturnValues(boolean r, int e) {
		this.result = r;
		this.errorCode = e;
	}
	
	public void setResult(boolean r) {
		result = r;
	}
	
	public void setErrorCode(int e) {
        errorCode = e;
	}
		
	public boolean getResult() {
		return result;
	}
	
	public int getErrorCode() {
		return errorCode;
	}
}