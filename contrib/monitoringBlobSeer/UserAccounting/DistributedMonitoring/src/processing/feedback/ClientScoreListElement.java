package processing.feedback;

/**
 * @author acarpena
 *
 * Class that implements a type of a malicious list element that has an associated score
 */
@SuppressWarnings("serial")
public class ClientScoreListElement extends BlackListElement {

	protected int score;	// associate a score with each client
	
	public ClientScoreListElement(String s) {
		super(s);
		score = 0;
	}

	public ClientScoreListElement(String s, int v){
		super(s);
		score = v;
	}

	public int getScore() {
		return score;
	}

	public void setScore(int score) {
		this.score = score;
	}
	
	public int compareTo(BlackListElement el)
	{
		return score - ((ClientScoreListElement)el).getScore();
	}
	
}