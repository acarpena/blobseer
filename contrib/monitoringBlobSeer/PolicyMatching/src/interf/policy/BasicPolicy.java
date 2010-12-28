/**
 * Security Policy in BlobSeer.
 * 
 * @author Cristina Basescu
 * @date 24.03.2010
 */

package interf.policy;


import interf.list.IList;
import interf.list.IListFactory;
import interf.xml.XMLElem;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.io.Serializable;
import java.util.HashSet;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMResult;
import javax.xml.transform.dom.DOMSource;

import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.ls.DOMImplementationLS;
import org.w3c.dom.ls.LSOutput;
import org.w3c.dom.ls.LSSerializer;

import util.alg.XMLWriter;
import util.conf.Conf;
import util.list.ListFactory;
import util.xml.Action;
import util.xml.Active;
import util.xml.ClientID;
import util.xml.EModule;
import util.xml.End;
import util.xml.Priority;
import util.xml.Start;
import util.xml.Action;

/*
 * Must have defined priority, id, whether it's active or not, start and end points and client id.
 */
public abstract class BasicPolicy implements Serializable, Cloneable {
	/**
	 * Used by Serializable.
	 */
	private static final long serialVersionUID = -7420964753067879147L;

	/** XML elements children of policy tag*/
	protected IList<XMLElem> rootElems;

	/** external data */
	protected EData externalData;
	
	/** preconditions */
	protected Precond precond;
	
	/** actions */
	protected Actions actions;
	
	/** factory object for creating lists */
	protected final IListFactory<XMLElem> f = new ListFactory<XMLElem>();
	
	/** hashset containing policy classes that the current policy can enable */
	protected HashSet<Integer> enabledPolicies;
	
	/** hashset containing the ids of all elements in the policy */
	public HashSet<String> elementIds;
	
	/** policy id: PolicyType_Conf.lastPolicyNum eg 1_5 */
	protected String id;
	
	/** XML document to store the XML representation */
	public Document dom;
	
	/** alg for XML conversion */
	private XMLWriter xmlWriter;
	
	/** boolean keeping track of setting start and end event for the policy */
	private Boolean startSet;
	private Boolean endSet;
	private String startId;
	private String endId;
	
	public Element startEvent = null;
	
	/** last event on which there was a match */
	public String lastMatchId = "";
	
	/** frequence of check in ms */
	public int frequence = -1; 
	
	public BasicPolicy clone()
    {
        try
        {
        	BasicPolicy bp = (BasicPolicy)super.clone();
        	TransformerFactory tfactory = TransformerFactory.newInstance();
        	Transformer tx   = tfactory.newTransformer();
        	DOMSource source = new DOMSource(dom);
        	DOMResult result = new DOMResult();
        	tx.transform(source,result);
        	bp.dom = (Document)result.getNode();
        	bp.hashCode();
        	return bp;
        }
    catch(Exception e )
    {
            return null;
        }
    } 
	
	/**
	 * Constructor
	 */
	protected BasicPolicy(String clientId, Boolean active, Integer priority) throws Exception {
		this.externalData = new EData();
		this.precond = new Precond();
		this.actions = new Actions();
		this.rootElems = f.makeNonEmpty(new ClientID(null, clientId, clientId), f.makeEmpty());
		this.rootElems = f.makeNonEmpty(new Active(null, active.toString()), this.rootElems);
		this.rootElems = f.makeNonEmpty(new Priority(null, priority.toString()), this.rootElems);
		
		this.xmlWriter = new XMLWriter();
		this.enabledPolicies = new HashSet<Integer>();
		this.elementIds = new HashSet<String>();
		this.id = null;
		
		startSet = false;
		endSet = false;
		startId = null;
		endId = null;
		
		// create DOM
		createDom();
	}
	
	/**
	 * sets the start event for a policy
	 * @param start event
	 * @throws Exception when start event 
	 */
	public void setStart(String elemId) throws Exception {
		if(!elementIds.contains(elemId))
			throw new Exception("Invalid element id as start point.");
		if(startSet)
			throw new Exception("Already set a start point.");
		this.rootElems = f.makeNonEmpty(new Start(null, elemId), this.rootElems);
		startSet = true;
		startId = elemId;
	}
	
	public void setEnd(String elemId) throws Exception {
		if(!elementIds.contains(elemId))
			throw new Exception("Invalid element id as start point.");
		if(endSet)
			throw new Exception("Already set an end point.");
		this.rootElems = f.makeNonEmpty(new End(null, elemId), this.rootElems);
		endSet = true;
		endId = elemId;
	}
	
	public String getStartId() {
		return startId;
	}
	
	public String getEndId() {
		return endId;
	}
	
	/**
	 * Builds XML representation of the policy.
	 */
	public void buildXML() throws Exception {
		try
		{
			// check whether the policy has a start event and an end event
			if(!startSet || !endSet)
				throw new Exception("Start event or end event not set.");
			
			if(this.id == null)
				throw new Exception("Policy id cannot be null.");
			
			if(frequence < 0)
				throw new Exception("Frequence can't be negative");
			
			// create root element
			Element rootEle = dom.createElement("securityPolicy");
			dom.appendChild(rootEle);
			
			// add id attribute
			rootEle.setAttribute("id", this.id);
			
			// add root elements
			rootElems.toXML(xmlWriter, rootEle, dom);
			
			// add precondition elements
			precond.buildXML(rootEle, dom, xmlWriter);
			
			// add action elements
			actions.buildXML(rootEle, dom, xmlWriter);
			
			// write
			write(Conf.policyDir+"securityPolicy_"+id+".xml");
		}catch(IOException ie) {
		    ie.printStackTrace();
		}
	}
		
	public void write(String filename) {
		try {
			//get a LSOutput object
			DOMImplementationLS domImplem = null;
			domImplem = (DOMImplementationLS)(dom.getImplementation()).getFeature("LS","3.0");
	        LSOutput lso = domImplem.createLSOutput();
	    
	        //setting the location for storing the result of serialization
	        FileOutputStream fos = new FileOutputStream(filename);
	        
	        lso.setByteStream((OutputStream)fos);          
	        
	        //get a LSSerializer object
	        LSSerializer lss = domImplem.createLSSerializer();
	    
	        //do the serialization
	        boolean ser = lss.write(dom,lso);
	    
	        //publish the result
	        if(ser)
	           System.out.println("[Serialization done!]");
	        else System.out.println("[Serialization failed!]");

		} catch(IOException ie) {
		    ie.printStackTrace();
		}
	}
	
	/**
	 * Serialize policy.
	 */
	public void serialize() {
		FileOutputStream fos = null;
		ObjectOutputStream out = null;
		try
		{
			fos = new FileOutputStream(Conf.serDir+"securityPolicy_"+id+".ser");
			out = new ObjectOutputStream(fos);
			out.writeObject(this);
			out.close();
		}
		catch(IOException ex)
		{
			ex.printStackTrace();
		}
	}
	
	/**
	 * Gives the possibility to have as an action the enable a certain type of policy 
	 * @param policyId
	 */
	protected void allowEnablePolicy(Integer type) {
		enabledPolicies.add(type);
	}
	
	/**
	 * Gets the policy type (from interf.policy.PolicyType)
	 * @return
	 */
	protected abstract Integer getPolicyType();
	
	/**
	 * Adds a precondition.
	 * @param elem the precondition
	 */
	public void addEData(EModule elem) {
		externalData.addModule(elementIds, elem);
	}
	
	/**
	 * Adds a precondition.
	 * @param elem the precondition
	 */
	public void addPrecond(XMLElem elem) {
		precond.addPrecond(elementIds, elem);
	}
	
	/**
	 * Adds an action
	 * @param elem the action
	 */
	public void addAction(Action elem) throws Exception {
		actions.addAction(elementIds, elem);
	}
	
	public EData getEData() {
		return externalData;
	}
	
	public Precond getPrecond() {
		return precond;
	}
	
	public Actions getActions() {
		return actions;
	}
	
	/**
	 * Created Document Object Model for storing the policy.
	 */
	private void createDom() {
		//get an instance of factory
		DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
		try {
			//get an instance of builder
			DocumentBuilder db = dbf.newDocumentBuilder();
	
			//create an instance of DOM
			this.dom = db.newDocument();
		}catch(ParserConfigurationException pce) {
			//dump it
			System.out.println("Error while trying to instantiate DocumentBuilder " + pce);
			System.exit(1);
		}
		
	}
	
	public Document getDom() {
		return dom;
	}
}
