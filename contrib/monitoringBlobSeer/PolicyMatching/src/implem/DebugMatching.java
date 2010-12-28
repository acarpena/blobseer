package implem;

import interf.policy.BasicPolicy;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.net.URL;
import java.sql.DatabaseMetaData;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.ReentrantLock;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import util.conf.Conf;
import util.db.DatabaseAccessJDBC;

public class DebugMatching {
	
	///debug vars
	int conns = 0;
	/////// debug vars end
	
	
	LinkedList<BasicPolicy> policies;
	static LinkedList<BasicPolicy> partiallyMatched = new LinkedList<BasicPolicy>();
	LinkedList<Integer> policyDelays;
	LinkedList<DatabaseAccessJDBC> dbc;
	DatabaseMetaData md;
	int databaseAccessIndex = -1;
	String dbpath;
	private static int MAX_INSTANCES = 10000000;
	
	private static final ReentrantLock match = new ReentrantLock(true);

	private static final Semaphore emptyCount = new Semaphore(MAX_INSTANCES,
			true);
	private static final Semaphore fillCount = new Semaphore(0, true);
	
	private static final Semaphore dbMutex = new Semaphore(1,
			true);

	public DebugMatching(String matching_conf, String dbpath) {
		Conf.confFile = matching_conf;
		Conf.readConf();
		this.dbpath=dbpath;
		policies = new LinkedList<BasicPolicy>();
		policyDelays = new LinkedList<Integer>();
		dbc = new LinkedList<DatabaseAccessJDBC>();
        System.out.println("Using db file: " + this.dbpath);
		for(int i = 0 ; i < 5 ;i++) {
			DatabaseAccessJDBC temp = new DatabaseAccessJDBC(this.dbpath);
			conns++;
			//temp.initializeDB();
			dbc.add(temp);
			
		}
		md = dbc.get(0).getMetaData();
		readPolicies(Conf.serDir);

		// get policy types
		try {
			for (BasicPolicy bp : policies) {
				Integer freq = bp.getClass().getField("frequence").getInt(bp);
				if (!policyDelays.contains(freq.intValue())) {
					policyDelays.add(freq);
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		Collections.sort(policyDelays);

		fillStartEvents();

	}

	// get unused db conn
	private DatabaseAccessJDBC getDatabaseAccess() {
		try {
			DebugMatching.dbMutex.acquire();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if(!dbc.isEmpty()) {
			DatabaseAccessJDBC rez = dbc.removeFirst();
			DebugMatching.dbMutex.release();
			return rez;
		}
		
		// if it's empty and the maximum number of allowed connections was created - 
		// wait until the number of used connections decreases
		if (conns >=90)
		{
			DebugMatching.dbMutex.release();
            try {
				Thread.sleep (1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return getDatabaseAccess();
		}
		
		DatabaseAccessJDBC temp = new DatabaseAccessJDBC(this.dbpath);
		conns++;
		//temp.initializeDB(); 
		dbc.add(temp);
		
		System.out.println(Thread.currentThread().getId() + "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5");
		System.out.println(Thread.currentThread().getId() + "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5");
		System.out.println(Thread.currentThread().getId() + "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5");
		System.out.println(Thread.currentThread().getId() + "intorc");
		System.out.println(Thread.currentThread().getId() + "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5");
		System.out.println(Thread.currentThread().getId() + "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5");
		System.out.println(Thread.currentThread().getId() + "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5");
		
		DatabaseAccessJDBC rez = dbc.removeFirst();
		DebugMatching.dbMutex.release();
		return rez;
	}
	
	// put db conn
	private synchronized void putDatabaseAccess(DatabaseAccessJDBC db) {
		dbc.add(db);
}
	
	// browse active policies and fill pointers to start events (startEvent
	// variable)
	private void fillStartEvents() {
		for (BasicPolicy bp : policies) {
			String id = bp.getStartId();
			System.out.println(Thread.currentThread().getId() + "start = " + id);
			NodeList sections = bp.getDom().getElementsByTagName("event");
			int numSections = sections.getLength();
			for (int i = 0; i < numSections; i++) {
				Element section = (Element) sections.item(i); // an event
				if (section.getAttribute("id").matches(id)) {
					bp.startEvent = section;
					System.out.println(Thread.currentThread().getId() + " " +section.getAttribute("type"));
				}
			}
		}
	}

	/**
	 * This method instantiates new policies and checks whether policies in progress have finished matching
	 */
	public void applyPolicies() {
		// start a new thread for each type of policy
		for (int delay : policyDelays) {
			PolicyMatch pm = new PolicyMatch(delay);
			pm.start();
		}
		
		// start the 'finished matching' thread
		PolicyCheck pc = new PolicyCheck();
		pc.start();
	}

	private LinkedList<String> getTables(String eventName) throws Exception {
		LinkedList<String> ret = new LinkedList<String>();

		ResultSet rs = md.getTables(null, null, "%", new String[] { "TABLE" });
		while (rs.next()) {
			// select keywords tables
			String name = rs.getString(3);
			//System.out.println(Thread.currentThread().getId() + "get tables name=" + name);
			if (name.contains(eventName)) {
				if (!ret.contains(name))
					ret.add(name);
			}
		}
		System.out.println(Thread.currentThread().getId() + "get tables end");
		return ret;
	}

	// returns a policy for each client, if match succeeded
	private LinkedList<BasicPolicy> matchEvent(BasicPolicy bp, Element crt,
			Boolean fillClientID, String distance, String addWhere, DatabaseAccessJDBC db,
			String table, int dbIndex) 
			throws Exception {
		
		// see whether there are lines in the database below this id
		Boolean ready = false;
		while(!ready) {
			System.out.println("[" + System.currentTimeMillis() + "] " 
					+ Thread.currentThread().getId() + " " +"///////////////////////////////O noua incercare curajoasa - cu flush");
			System.out.flush();
			String tempQuery = "SELECT * FROM \"" + table + "\" WHERE id >= " + dbIndex;
			System.out.println(Thread.currentThread().getId() + " " +tempQuery);
			db.query(tempQuery);
			
			if(db.next())
				ready = true;
			else
			{	
				this.putDatabaseAccess(db);
				//db.close();
				// try again every 10 seconds
				Thread.sleep(3000);
				db = getDatabaseAccess();
			}
		}
		
		match.lock();
		
		System.out.println(Thread.currentThread().getId()+" aici hash code politica "+bp.hashCode()+" si elem "+crt.hashCode());
		
		
		LinkedList<BasicPolicy> result = new LinkedList<BasicPolicy>();
		
		System.out.println(Thread.currentThread().getId() + " " +"%%%%% NUME EVEN "+crt.getTagName()+ " id "+ crt.getAttribute("id") + 
				" tip "+ crt.getAttribute("type"));
		
		String firstDateVal = null, lastDateVal = null;
		

		// get interesting columns
		NodeList children = crt.getChildNodes();
		int numChildren = children.getLength();
		LinkedList<String> columns = new LinkedList<String>();
		LinkedList<String> continuous = new LinkedList<String>();
		Hashtable<String, String> where = new Hashtable<String, String>();
		for (int j = 0; j < numChildren; j++) {

			// get fields to be included in select clause - all but clientId,
			// continuous, precededBy, followedBy or any other field that has
			// value set,
			// but not rvalue
			Element child = (Element) children.item(j);
			String elemName = child.getTagName();
			if (elemName.compareTo("clientID") != 0
					&& elemName.compareTo("continuous") != 0
					&& elemName.compareTo("and") != 0
					&& elemName.compareTo("or") != 0
					&& elemName.compareTo("neg") != 0
					&& elemName.compareTo("precededBy") != 0
					&& elemName.compareTo("followedBy") != 0) {
				if (!(child.getAttribute("rvalue") == "" && child
						.getAttribute("value") != "")) {
					if(!elemName.contains("count")) {
						columns.add(elemName);
					}
				}
			}

			// continuous
			HashSet<String> contList = new HashSet<String>();
			if (elemName.compareTo("continuous") == 0) {
				// get all children
				NodeList cont = child.getChildNodes();
				int numcont = cont.getLength();
				for (int k = 0; k < numcont; k++) {
					Element c = (Element) cont.item(k);
					String valCont = c.getAttribute("value");
					contList.add(valCont);
					
					
				}
			}
			// search the name of the xml elem with this id
			NodeList refCont = crt.getChildNodes();
			int numRefCont = refCont.getLength();
			for (int k = 0; k < numRefCont; k++) {
				Element c = (Element) refCont.item(k);
				String idCont = c.getAttribute("id");
				if(contList.contains(idCont))
					continuous.add(c.getTagName());
			}
				
				
			//System.out.println("DAAAAAAAAAAAAAAAAAAAAAAAAAAAAA "+continuous.toString());
			
			// what the clause WHERE will contain - analyze 
			// all the fields that have value, but not rvalue, and are in the
			// table schema and those for which rvalue is not empty
			
			if (child.getAttribute("value") != "") {
				System.out.println(Thread.currentThread().getId() + " " +"procesez "+child.getTagName());
				// daca nu exista atributul rvalue
				if(child.getAttribute("rvalue") == "") {
					System.out.println(Thread.currentThread().getId() + " " +"tagname "+child.getTagName()+" e rvalue vid");
					String val = child.getAttribute("value");
					String processed = processValue(bp.getDom(), val);
					// add ' '
					StringTokenizer st = new StringTokenizer(processed, Conf.opConstants);
					String elem = st.nextToken();
					int index = processed.indexOf(elem);
					where.put(elemName.toLowerCase(), processed.substring(0, index) + '\''+elem+'\'');
					
				}
				else {
					// daca exista atributul rvalue si nu e vid
					if(child.getAttribute("rvalue").compareTo("") != 0) {
						System.out.println(Thread.currentThread().getId() + " " +"nevid");
						String val = child.getAttribute("rvalue");
						String processed = processValue(bp.getDom(), val);
						StringTokenizer st = new StringTokenizer(processed, Conf.opConstants);
						String elem = st.nextToken();
						int index = processed.indexOf(elem);
						where.put(elemName.toLowerCase(), processed.substring(0, index) + '\''+elem+'\'');
					}
				}
			}

		}

		System.out.println(Thread.currentThread().getId() + " " +"DAAAAAAAAAAAAAAAAAAAAAAAAAAAAA "+continuous.toString());
		
		// System.out.println(crt.getTagName());
		// System.out.println(columns.toString());
		System.out.println(Thread.currentThread().getId() + " " +"-----------" + where.toString());
		System.out.println(Thread.currentThread().getId() + " " +"-----------" + where.values().toString());

		
		
		
		// build select query on each table
		BasicPolicy res = bp;
		
		// check that it's for the same client
		if(!fillClientID) {
			// get the first clientID
			NodeList s = bp.getDom().getElementsByTagName(
					"clientID");
			Element sct = (Element) s.item(0);
			String val = sct.getAttribute("rvalue");
			System.out.println(Thread.currentThread().getId() + " clientID = "+val+" si tabela e "+table);
			if(!table.contains(val)) {
				System.out.println(Thread.currentThread().getId()+" intorc null");
				match.unlock();
            	this.putDatabaseAccess(db);
				return result;
			}
		}
		

		// get columns and adjust where clause
		Boolean hasFirstDate = false;
		Boolean hasLastDate = false;
		LinkedList<String> whereAux = new LinkedList<String>();
		ResultSet resultSet = md.getColumns(null, null, table, null);
		while (resultSet.next()) {
			String name = resultSet.getString("COLUMN_NAME");
			System.out.println(Thread.currentThread().getId() + " " +name);
			if(name.contains("firstdate"))
				hasFirstDate = true;
			if(name.contains("lastdate"))
				hasLastDate = true;
			if (where.containsKey(name))
				whereAux.add(name + where.get(name));
		}
		
		System.out.println(Thread.currentThread().getId() + " " +"whereaux "+whereAux.toString() );
		
		// build query
		String query = "SELECT ";
		for (int j = 0; j < columns.size(); j++) {
			if (j == columns.size() - 1)
				query += columns.get(j) + " ";
			else
				query += columns.get(j) + ", ";
		}

		query += "FROM \"" + table + "\" ";
		
		
		query += "WHERE id >= " + dbIndex + " ";
		
		if(distance != null) {
			// find timestamp: lastdate or firstDate
			if(hasLastDate)
				query += " AND CAST(lastDate as BIGINT) " + distance +" ";
			else if(hasFirstDate)
				query += " AND CAST(firstDate as BIGINT) " + distance + " ";
			System.out.println(Thread.currentThread().getId() + " " +"aici");
		}

		
		
		if (whereAux.size() > 0) {
			
			
			
			if(whereAux.size() > 0)
				query += " AND ";
			
			for (int j = 0; j < whereAux.size(); j++) {
				if (j == whereAux.size() - 1)
					query += whereAux.get(j) + " ";
				else
					query += whereAux.get(j) + " AND ";
			}
		}
		
		if(addWhere != null) {
			
			if(hasLastDate)
				query += " AND CAST(lastDate as BIGINT) " + addWhere + " ";
			else if(hasFirstDate)
				query += " AND CAST(firstDate as BIGINT) " + addWhere +" ";		
		}

		
		if(hasFirstDate && hasLastDate)
			query += "ORDER BY firstDate, lastDate, id";
		else {
			if(hasFirstDate && !hasLastDate)
				query += "ORDER BY firstDate, id";
			else {
				if(hasLastDate && !hasFirstDate)
					query += "ORDER BY lastDate, id";
			}
		}
		
		System.out.println(Thread.currentThread().getId() + " " +"query = " + query);

		db.query(query);
		int index = 0;
		
		boolean queryReturned = false;
		// matched at least once on this client
		Boolean matched = false;
		while (db.next()) {
			queryReturned = true;
			System.out.println(Thread.currentThread().getId()+" index = "+index);
			System.out.println(Thread.currentThread().getId() + " " +"DBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB in "+crt.getAttribute("id"));
			System.out.println(Thread.currentThread().getId() + " " +"(((((((((((((((((((((((((((((((((((( continuous "+continuous.toString());
			if (index == 0) {
				if (fillClientID) {
					System.out.println(Thread.currentThread().getId() + " " +"fill");
					StringTokenizer st = new StringTokenizer(table, "+");
					st.nextToken();
					String client = st.nextToken();
					// get the first clientID and replace
					NodeList s = bp.getDom().getElementsByTagName(
							"clientID");
					Element sct = (Element) s.item(0);
					String val = sct.getAttribute("value");
					sct.setAttribute("rvalue", client);
					System.out.println(Thread.currentThread().getId()+" filled with "+client);
					// replace all clientID with value val
					res = replaceRvalue(res, val, client);
				}
			}
			
			// build a temp one if conditions fail
			//BasicPolicy temp = res;
			
			//res.write(Conf.policyDir + Thread.currentThread().getId()+" begin_temp_"+crt.getAttribute("id")+" "+index+" ");
			
			// fill in all selected fields that are in the event
			for(String column : columns) {
				// don't fill the counts
				if(column.toLowerCase().contains("count"))
					continue;
				
				// if it's not the first row, fill only continuous
				
				if((index != 0 && continuous.contains(column)) || index == 0) {
					System.out.println(Thread.currentThread().getId() + " " +"column="+column);
					/*
					String val = null;
					NodeList s = bp.getDom().getElementsByTagName(
							"*");
					for(int l = 0 ; l < s.getLength(); l++) {
						Element sct = (Element) s.item(l);
						if(sct == crt) {
							val = sct.getAttribute("value");
							sct.setAttribute("rvalue", db.getString(column));
						}
					}
					*/
					String val = null;
					NodeList s = crt.getElementsByTagName(column);
					for(int l = 0 ; l < s.getLength(); l++) {
						System.out.println(Thread.currentThread().getId() + " " +"%%$%^$^%$%^");
						Element sct = (Element) s.item(l);
						
							val = sct.getAttribute("value");
							sct.setAttribute("rvalue", db.getString(column));
						
					}
					if(val != "" && val.compareTo("") != 0) {
						
						System.out.println(Thread.currentThread().getId() + " " +"column="+column+" value="+val+" rvalue="+db.getString(column));
						res = replaceRvalue(res, val, db.getString(column));
					}
				}
		
			}
			
			// find firstdate val, lastdate val
			if(hasFirstDate)
				firstDateVal = db.getString("firstDate");
			
			if(hasLastDate)
				lastDateVal = db.getString("lastDate");
			
			
			
			// ---- count
			
			System.out.println(Thread.currentThread().getId() + " " +"aieaaaaaa");
			//res.write(Conf.policyDir + Thread.currentThread().getId()+ "temp"+crt.getAttribute("id")+" "+index);
			
			
			
			// look for conditionals, preceded by, followed by
			
			// create a copy of the policy so that to preserve the initial one
			BasicPolicy temp = (BasicPolicy)res.clone();
			//temp.dom = (Document)res.dom.cloneNode(true);
			
			Boolean conds = true;
			NodeList c = crt.getChildNodes();
			int numc = c.getLength();
			
			
			for (int k = 0; k < numc; k++) {
				
				Element child = (Element) c.item(k);
				String elemName = child.getTagName();
				System.out.println(Thread.currentThread().getId() + " " +"-----> "+elemName);
				if (elemName.compareTo("neg") == 0)
					conds = conds & processNot(temp, child, firstDateVal, lastDateVal);
				if (elemName.compareTo("and") == 0)
					conds = conds & processAnd(temp, child, firstDateVal, lastDateVal);
				if (elemName.compareTo("or") == 0)
					conds = conds & processOr(temp, child, firstDateVal, lastDateVal);
				if (elemName.compareTo("precededBy") == 0)
					conds = conds & processPrecBy(temp, child, firstDateVal, lastDateVal);
				if (elemName.compareTo("followedBy") == 0)
					conds = conds & processFolBy(temp, child, firstDateVal, lastDateVal);
			}	
			
			// if conditions pass this is the final one
			if(numc > 0) {
				System.out.println(Thread.currentThread().getId() + " " +"===========================================================");
				System.out.println(Thread.currentThread().getId() + " " +"+++++++++ conds="+conds.toString()+" pt "+crt.getAttribute("id"));
				System.out.println(Thread.currentThread().getId() + " " +"===========================================================");
			}
			
			// fill in the counts
			System.out.println(Thread.currentThread().getId()+" %% hash crt "+crt.hashCode());
			System.out.println(Thread.currentThread().getId() + " " +"elem curent "+crt.getTagName()+ "id="+crt.getAttribute("id"));
			NodeList s = crt.getChildNodes();
			for(int l = 0 ; l < s.getLength(); l++) {
				//System.out.println(Thread.currentThread().getId() + " " +"SUTN IN FOR");
				Element sct = (Element) s.item(l);
				if(sct.getTagName().toLowerCase().contains("count"))
					if(sct.getTagName().toLowerCase().compareTo("count") == 0) {
						System.out.println(Thread.currentThread().getId() + " " +"============> suntem pe cazul cu count index = "+index);
						System.out.println(Thread.currentThread().getId() + " " +"=== " + (index+1));
						if(conds) {
							sct.setAttribute("rvalue", (index+1)+"");
							System.out.println(Thread.currentThread().getId() + " " +"completez rvalue cu "+(index+1));
						}
						else {
							sct.setAttribute("rvalue", "0");
							System.out.println(Thread.currentThread().getId() + " " +"completez rvalue cu 0");
						}
					}
					else {
						Long oldVal = new Long(0);
						if(sct.getAttribute("rvalue").compareTo("") != 0)
							oldVal = Long.parseLong(sct.getAttribute("rvalue"));
						Long inc = Long.parseLong(db.getString(sct.getTagName()));
						Long newVal = oldVal + inc;
						if(conds) {
							sct.setAttribute("rvalue", newVal.toString());
							System.out.println(Thread.currentThread().getId() + " rvalue a fost " +oldVal+" si devine "+newVal);
						}
						
					}
							
					
				
				if(conds)
					matched = true;
				//res = temp;
			}
		
			
			index++;

		}
		// the client finished 
		if(matched) {
			result.add(res);
			System.out.println(Thread.currentThread().getId() + " " +"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! il adaug");
		}
		
		if(!queryReturned){
			// fill in the counts
			NodeList s = crt.getChildNodes();
			for(int l = 0 ; l < s.getLength(); l++) {
				System.out.println(Thread.currentThread().getId() + " " +"SUTN IN FOR");
				Element sct = (Element) s.item(l);
				if(sct.getTagName().toLowerCase().contains("count"))
					if(sct.getTagName().toLowerCase().compareTo("count") == 0) {
						sct.setAttribute("rvalue", "0");
						System.out.println(Thread.currentThread().getId() + " " +"completez rvalue cu 0");
					}
				
			}
		}

		match.unlock();
		System.out.println(Thread.currentThread().getId() + " " +"^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ies din "+crt.getAttribute("id"));
		this.putDatabaseAccess(db);
		return result;
	}

	private Boolean processNot(BasicPolicy temp, Element elem, String firstDateId, String lastDateId) 
	throws Exception {
		
		Boolean conds = true;
		System.out.println(Thread.currentThread().getId() + " " +"process not tag "+elem.getTagName()+" cu conds "+conds);
		NodeList c = elem.getChildNodes();
		int numc = c.getLength();
		for (int j = 0; j < numc; j++) {
			Element child = (Element) c.item(j);
			String elemName = child.getTagName();
			if (elemName.compareTo("neg") == 0) {
				conds = conds & processNot(temp, child, firstDateId, lastDateId);
				System.out.println(Thread.currentThread().getId() + " " +"dupa neg conds = "+conds);
			}
			if (elemName.compareTo("and") == 0) {
				conds = conds & processAnd(temp, child, firstDateId, lastDateId);
				System.out.println(Thread.currentThread().getId() + " " +"dupa and conds = "+conds);
			}
			if (elemName.compareTo("or") == 0) {
				conds = conds & processOr(temp, child, firstDateId, lastDateId);
				System.out.println(Thread.currentThread().getId() + " " +"dupa or conds = "+conds);
			}
			if (elemName.compareTo("precededBy") == 0) {
				conds = conds & processPrecBy(temp, child, firstDateId, lastDateId);
				System.out.println(Thread.currentThread().getId() + " " +"dupa prec conds = "+conds);
			}
			if (elemName.compareTo("followedBy") == 0) {
				conds = conds & processFolBy(temp, child, firstDateId, lastDateId);
				System.out.println(Thread.currentThread().getId() + " " +"dupa fol conds = "+conds);
			}
		}
		System.out.println(Thread.currentThread().getId() + " " );
		System.out.println(Thread.currentThread().getId() + " " );
		System.out.println(Thread.currentThread().getId() + " " +"++++++rez neg "+new Boolean((!conds)).toString());
		System.out.println(Thread.currentThread().getId() + " " );
		System.out.println(Thread.currentThread().getId() + " " );
		return !conds;
	}
	
	private Boolean processAnd(BasicPolicy temp, Element elem, String firstDateId, String lastDateId) 
		throws Exception {
		System.out.println(Thread.currentThread().getId() + " " +"process and");
		Boolean conds = true;
		NodeList c = elem.getChildNodes();
		int numc = c.getLength();
		for (int j = 0; j < numc; j++) {
			Element child = (Element) c.item(j);
			String elemName = child.getTagName();
			if (elemName.compareTo("neg") == 0)
				conds = conds & processNot(temp, child, firstDateId, lastDateId);
			if (elemName.compareTo("and") == 0)
				conds = conds & processAnd(temp, child, firstDateId, lastDateId);
			if (elemName.compareTo("or") == 0)
				conds = conds & processOr(temp, child, firstDateId, lastDateId);
			if (elemName.compareTo("precededBy") == 0)
				conds = conds & processPrecBy(temp, child, firstDateId, lastDateId);
			if (elemName.compareTo("followedBy") == 0)
				conds = conds & processFolBy(temp, child, firstDateId, lastDateId);
		}	
		return conds;
	}
	
	private Boolean processOr(BasicPolicy temp, Element elem, String firstDateId, String lastDateId) 
	throws Exception {
		System.out.println(Thread.currentThread().getId() + " " +"process or");
		Boolean conds = false;
		NodeList c = elem.getChildNodes();
		int numc = c.getLength();
		for (int j = 0; j < numc; j++) {
			Element child = (Element) c.item(j);
			String elemName = child.getTagName();
			if (elemName.compareTo("neg") == 0)
				conds = conds | processNot(temp, child, firstDateId, lastDateId);
			if (elemName.compareTo("and") == 0)
				conds = conds | processAnd(temp, child, firstDateId, lastDateId);
			if (elemName.compareTo("or") == 0)
				conds = conds | processOr(temp, child, firstDateId, lastDateId);
			if (elemName.compareTo("precededBy") == 0)
				conds = conds | processPrecBy(temp, child, firstDateId, lastDateId);
			if (elemName.compareTo("followedBy") == 0)
				conds = conds | processFolBy(temp, child, firstDateId, lastDateId);
		}	
		return conds;
	}
	
	private Boolean processPrecBy(BasicPolicy temp, Element elem, String firstDateVal, String lastDateVal) 
	throws Exception {
		System.out.println(Thread.currentThread().getId() + " " +"process prec");
		NodeList c = elem.getChildNodes();
		int numc = c.getLength();
		Element refEvent = null, count = null, distance = null;
		for (int j = 0; j < numc; j++) {
			Element child = (Element) c.item(j);
			String elemName = child.getTagName();
			if(elemName.compareTo("refEvent") == 0) {
				refEvent = child;
			}
			if(elemName.compareTo("count") == 0) {
				count = child;
			}
			if(elemName.compareTo("distance") == 0) {
				distance = child;
			}
		}
		
		// get distance
		String dist = null;
		if(distance.getAttribute("rvalue") != "" && distance.getAttribute("rvalue").compareTo("") != 0) {
			System.out.println(Thread.currentThread().getId() + " " +"dist rvalue "+distance.getAttribute("rvalue"));
			dist = processValue(temp.getDom(), distance.getAttribute("rvalue"));
			distance.setAttribute("rvalue", dist);
		}
		else {
			System.out.println(Thread.currentThread().getId() + " " +"dist value "+distance.getAttribute("value"));
			dist = processValue(temp.getDom(), distance.getAttribute("value"));
			distance.setAttribute("value", dist);
		}
		
		System.out.println(Thread.currentThread().getId() + " " +"dist final " + dist);
		
		// additional distance
		String addDist = null;
		if(firstDateVal != null)
			addDist =  "<= "+firstDateVal;
		else
			addDist =  "<= "+lastDateVal;
		String addDistVal = processValue(temp.getDom(), addDist);
		
		// match on this event
		String idRefEvent = refEvent.getAttribute("value");
		// search this element and match on it
		NodeList s = temp.getDom().getElementsByTagName("*");
		int ns = s.getLength();
		for (int i = 0; i < ns; i++) {
			Element sct = (Element) s.item(i);
			String id = sct.getAttribute("id");
			if(id.compareTo(idRefEvent) == 0) {
				LinkedList<String> tableNames = getTables(sct.getAttribute("type"));
				for(String tableName: tableNames) {
					System.out.println(Thread.currentThread().getId() + " " +"da deci aici e faza cu durere "+sct.getAttribute("type"));
					DatabaseAccessJDBC db_conn = this.getDatabaseAccess();
					matchEvent(temp, sct, false, dist, addDistVal, db_conn, tableName, 0);
					
				}
				break;
			}
		}
		
		// get the count in referred policy
		String refCount = "";
		NodeList sc = temp.getDom().getElementsByTagName("*");
		int nsc = sc.getLength();
		for (int i = 0; i < nsc; i++) {
			Element sct = (Element) sc.item(i);
			String id = sct.getAttribute("id");
			if(id.compareTo(idRefEvent) == 0) {
				// get its children and search for count
				NodeList scc = sct.getElementsByTagName("count");
				Element scct = (Element) scc.item(0);
				refCount = scct.getAttribute("rvalue");
				if(refCount.compareTo("") == 0)
					refCount = "0";
			}
		}
		
		
		// process count
		String size = null;
		if(count.getAttribute("rvalue") != "" && count.getAttribute("rvalue").compareTo("") != 0) {
	
			size = processValue(temp.getDom(), count.getAttribute("rvalue"));
			System.out.println(Thread.currentThread().getId() + " " +"de la count il iau pe rvalue = "+count.getAttribute("rvalue"));
		}
		else {
			size = processValue(temp.getDom(), count.getAttribute("value"));
			System.out.println(Thread.currentThread().getId() + " " +"de la count il iau pe value");
		}
		
		System.out.println(Thread.currentThread().getId() + " " +"compar "+Long.parseLong(refCount) +" "+size);
		
		if(size.contains("!=")) {
			StringTokenizer st = new StringTokenizer(size, "!= ");
			if(Long.parseLong(refCount) != Long.parseLong(st.nextToken()))
				return true;
		}
		
		if(size.contains("<")) {
			StringTokenizer st = new StringTokenizer(size, "< ");
			if(Long.parseLong(refCount) < Long.parseLong(st.nextToken()))
				return true;
		}
		
		if(size.contains(">")) {
			StringTokenizer st = new StringTokenizer(size, "> ");
			if(Long.parseLong(refCount) > Long.parseLong(st.nextToken()))
				return true;
		}
		
		if(size.contains("=")) {
			StringTokenizer st = new StringTokenizer(size, "= ");
			if(Long.parseLong(refCount) == Long.parseLong(st.nextToken()))
				return true;
		}
		
		if(size.contains("<=")) {
			StringTokenizer st = new StringTokenizer(size, "< ");
			if(Long.parseLong(refCount) <= Long.parseLong(st.nextToken()))
				return true;
		}
		
		if(size.contains(">=")) {
			StringTokenizer st = new StringTokenizer(size, "> ");
			if(Long.parseLong(refCount) >= Long.parseLong(st.nextToken()))
				return true;
		}
		
		System.out.println(Thread.currentThread().getId() + " " +"intorc FALSE");
		return false;
	}
	
	private Boolean processFolBy(BasicPolicy temp, Element elem, String firstDateVal, String lastDateVal) 
	throws Exception {
		System.out.println(Thread.currentThread().getId() + " " +"process fol elem id = "+elem.getAttribute("id"));
		NodeList c = elem.getChildNodes();
		int numc = c.getLength();
		Element refEvent = null, count = null, distance = null;
		for (int j = 0; j < numc; j++) {
			Element child = (Element) c.item(j);
			String elemName = child.getTagName();
			if(elemName.compareTo("refEvent") == 0) {
				refEvent = child;
			}
			if(elemName.compareTo("count") == 0) {
				count = child;
			}
			if(elemName.compareTo("distance") == 0) {
				distance = child;
			}
		}
		
		// get distance
		String dist = null;
		if(distance.getAttribute("rvalue") != "" && distance.getAttribute("rvalue").compareTo("") != 0)
			dist = processValue(temp.getDom(), distance.getAttribute("rvalue"));
		else
			dist = processValue(temp.getDom(), distance.getAttribute("value"));
		
		// additional distance
		String addDist = null;
		if(lastDateVal != null)
			addDist =  ">= "+lastDateVal;
		else
			addDist =  ">= "+firstDateVal;
		String addDistVal = processValue(temp.getDom(), addDist);
		
		// match on this event
		String idRefEvent = refEvent.getAttribute("value");
		// search this element and match on it
		NodeList s = temp.getDom().getElementsByTagName("*");
		int ns = s.getLength();
		for (int i = 0; i < ns; i++) {
			Element sct = (Element) s.item(i);
			String id = sct.getAttribute("id");
			if(id.compareTo(idRefEvent) == 0) {
				System.out.println(Thread.currentThread().getId() + " " +"da deci aici e faza cu durere "+sct.getAttribute("type"));
				LinkedList<String> tableNames = getTables(sct.getAttribute("type"));
				for(String tableName: tableNames) {
					DatabaseAccessJDBC db_conn = this.getDatabaseAccess();
					matchEvent(temp, sct, false, dist, addDistVal, db_conn, tableName, 0);
					
				}
				break;
			}
		}
		
		// get the count in referred policy
		String refCount = "";
		NodeList sc = temp.getDom().getElementsByTagName("*");
		int nsc = sc.getLength();
		for (int i = 0; i < nsc; i++) {
			Element sct = (Element) sc.item(i);
			String id = sct.getAttribute("id");
			if(id.compareTo(idRefEvent) == 0) {
				// get its children and search for count
				NodeList scc = sct.getElementsByTagName("count");
				Element scct = (Element) scc.item(0);
				refCount = scct.getAttribute("rvalue");
				if(refCount.compareTo("") == 0)
					refCount = "0";
			}
		}
		
		
		// process count
		String size = null;
		if(count.getAttribute("rvalue") != "" && count.getAttribute("rvalue").compareTo("") != 0) {
			size = processValue(temp.getDom(), count.getAttribute("rvalue"));
			System.out.println(Thread.currentThread().getId() + " " +"de la count il iau pe rvalue = "+count.getAttribute("rvalue"));
		}
		else {
			size = processValue(temp.getDom(), count.getAttribute("value"));
			System.out.println(Thread.currentThread().getId() + " " +"de la count il iau pe value");
		}
		
		System.out.println(Thread.currentThread().getId() + " " +"compar "+Long.parseLong(refCount) +" "+size);
		
		if(size.contains("!=")) {
			StringTokenizer st = new StringTokenizer(size, "!= ");
			if(Long.parseLong(refCount) != Long.parseLong(st.nextToken()))
				return true;
		}
		
		if(size.contains("<")) {
			StringTokenizer st = new StringTokenizer(size, "< ");
			if(Long.parseLong(refCount) < Long.parseLong(st.nextToken()))
				return true;
		}
		
		if(size.contains(">")) {
			StringTokenizer st = new StringTokenizer(size, "> ");
			if(Long.parseLong(refCount) > Long.parseLong(st.nextToken()))
				return true;
		}
		
		if(size.contains("=")) {
			StringTokenizer st = new StringTokenizer(size, "= ");
			if(Long.parseLong(refCount) == Long.parseLong(st.nextToken()))
				return true;
		}
		
		if(size.contains("<=")) {
			StringTokenizer st = new StringTokenizer(size, "< ");
			if(Long.parseLong(refCount) <= Long.parseLong(st.nextToken()))
				return true;
		}
		
		if(size.contains(">=")) {
			StringTokenizer st = new StringTokenizer(size, "> ");
			if(Long.parseLong(refCount) >= Long.parseLong(st.nextToken()))
				return true;
		}
		
		System.out.println(Thread.currentThread().getId() + " " +"intorc FALSE");
		return false;
	}
	
	private String processValue(Document dom, String val) {
		String retVal = "";
		System.out.println(Thread.currentThread().getId() + " " +"$$$ val="+val);
		StringTokenizer st = new StringTokenizer(val, " " + Conf.opConstants);
		int index = 0;
		while(st.hasMoreTokens()) {
			// add "=" sign if necessary
			String token = st.nextToken();
			if(index == 0) {
				if(!val.startsWith(token))
					retVal = val;
				else retVal = "=" + val;
			}
			
			try {
				Long.parseLong(token);
			} catch (Exception e) {
				// if it's not a number, it means it's an id, so look for it in the document
				System.out.println(Thread.currentThread().getId() + " " +"$$$$$$$ token "+token);
				NodeList s = dom.getElementsByTagName("*");
				int ns = s.getLength();
				for (int i = 0; i < ns; i++) {
					Element sct = (Element) s.item(i);
					String id = sct.getAttribute("id");
					if(id.compareTo(token) == 0) {
						String numVal = sct.getAttribute("rvalue");
						if(numVal.compareTo("") == 0  || numVal == "" ) {
							numVal = sct.getAttribute("value");
						}
						// replace value in retVal
						System.out.println(Thread.currentThread().getId() + " " +"$$$$ numval = "+numVal);
						System.out.println(Thread.currentThread().getId() + " " +"$$$$ retval = "+retVal);
						System.out.println(Thread.currentThread().getId() + " " +"$$$$ token = "+token);
						retVal = retVal.replace(token, numVal);
						System.out.println(Thread.currentThread().getId() + " " +"$$$$ retval = "+retVal);
						break;
					}
					
				}
			}
			index++;
		}
		return retVal;
	}

	private BasicPolicy replaceRvalue(BasicPolicy bp, String value,
			String rvalue) {
		System.out.println(Thread.currentThread().getId() + " " +"aici");
		if (bp == null)
			System.out.println(Thread.currentThread().getId() + " " +"%");
		NodeList s = bp.getDom().getElementsByTagName("*");
		int ns = s.getLength();
		for (int i = 0; i < ns; i++) {
			Element sct = (Element) s.item(i);
			String val = sct.getAttribute("value");
			if (val.compareTo(value) == 0)
				sct.setAttribute("rvalue", rvalue);
		}
		System.out.println(Thread.currentThread().getId() + " " +"b;a " + Conf.policyDir);
		//bp.write(Conf.policyDir + "aha2");
		return bp;
	}

	private boolean checked(BasicPolicy bp) {
		
		Element crt = null;
		String endId = bp.getEndId();;
		System.out.println(Thread.currentThread().getId() + " " +bp.lastMatchId);
		if (bp.lastMatchId.compareTo("") != 0) {
			// match beginning with end event
			endId = bp.getEndId();
		}

		// analyze current event
		NodeList sections = bp.getDom().getElementsByTagName("event");
		int numSections = sections.getLength();
		for (int i = 0; i < numSections; i++) {
			Element section = (Element) sections.item(i);
			if(section.getAttribute("id").compareTo(endId) == 0) {
				Boolean rez = true;
				NodeList children = section.getChildNodes();
				int numChildren = children.getLength();
				for(int j = 0 ; j < numChildren ; j++) {
					Element child = (Element) children.item(j);
					if(child.getTagName().compareTo("content") == 0) {
						String value = child.getAttribute("value");
						String processed = processValue(bp.getDom(), value);
						
						// evaluate processed value
						System.out.println(Thread.currentThread().getId() + " " +"evaluating "+processed);
						return evalBoolean(processed);
						
					}
				}
				
			}

		}
		return false;
	}
	
	
	
	/**
	 * Inner class that starts instantiating policies
	 * @author cristina
	 *
	 */
	private class PolicyMatch extends Thread {
		int delay;

		public PolicyMatch(int delay) {
			super();
			this.delay = delay;
			System.out.println(Thread.currentThread().getId() + "New policyMatch delay=" + delay);
		}

		// instantiates new policies to working queue
		public void work() throws Exception {
			
			System.out.println(Thread.currentThread().getId() + "PolicyMatch work no_policies=" + policies.size());
			// choose new policies corresponding to this delay
			for (BasicPolicy bp : policies) {
				if (bp.frequence == delay) {
					System.out.println(Thread.currentThread().getId() + "connections=" + dbc.size());
					System.out.println(Thread.currentThread().getId() + "o politica start=" + bp.startEvent);					
					// get start event - see where to start matching from
					Element crt = bp.startEvent;
					Element crtPtr = null;
					System.out.println(Thread.currentThread().getId() + "o politica crt type=" + crt.getAttribute("type"));
					// get all tables corresponding to start event
					LinkedList<String> tableNames = getTables(crt.getAttribute("type"));
					
					// get start event in crtPtr
					NodeList sections = bp.getDom().getElementsByTagName("event");
					int numSections = sections.getLength();
					for (int i = 0; i < numSections; i++) {
						Element section = (Element) sections.item(i);
						if(section.getAttribute("type").compareTo("check") != 0) {
							if (section == crt) {
								crtPtr = section;
							}
						}
					}
					System.out.println(Thread.currentThread().getId() + "o politica tables=" + tableNames.size());
					// start a new thread for each table that matches the current event			
					for(String tableName: tableNames) {
						System.out.println(Thread.currentThread().getId() + "o politica are tablename=" + tableName);
						BasicPolicy bpAux = (BasicPolicy)bp.clone();
						//bpAux.dom = (Document)bp.dom.cloneNode(true);
						
						DoMatch dm = new DoMatch(bpAux, crtPtr, tableName, delay);
						dm.start();
					}
				}
			}
			System.out.println(Thread.currentThread().getId() + "Policy match work - dupa for" );
		}

		public void run() {
			try{
				work();
			} catch (Exception e) {
				System.out.println("In Policy Match catch");
				e.printStackTrace();
			}
		}
	}

	private class DoMatch extends Thread {
	
		BasicPolicy bp;
		Element section;
		String tableName;
		int delay;
		AtomicInteger PROGRESS_DB = new AtomicInteger(0);	// table index
		
		public DoMatch (BasicPolicy bp, Element section, String tableName, int delay) {
			this.bp = bp;
			
			System.out.println(Thread.currentThread().getId() + " " +"bp din Do Match e"+bp.hashCode());
			
			// get start event in crtPtr
			NodeList sections = bp.getDom().getElementsByTagName("event");
			int numSections = sections.getLength();
			for (int i = 0; i < numSections; i++) {
				Element s = (Element) sections.item(i);
				if(section.getAttribute("type").compareTo("check") != 0) {
					if (s.getAttribute("id").compareTo(section.getAttribute("id")) == 0) {
						this.section = s;
					}
				}
			}
			
			
			this.tableName = tableName;
			this.delay = delay;
		}
	
		public void work() throws Exception {
			
			System.out.println(Thread.currentThread().getId() + " " +"la inceput in Do Match am " + Thread.activeCount());
			System.out.println(Thread.currentThread().getId() + " " +"la inceput in Do Match am connections=" + conns);
			PROGRESS_DB.addAndGet(1);
			LinkedList<BasicPolicy> matched = new LinkedList<BasicPolicy>();
			BasicPolicy bpaux = bp;
			DatabaseAccessJDBC db_connection = getDatabaseAccess();
			matched.addAll(matchEvent(bpaux, section, true, null, null, db_connection, tableName, PROGRESS_DB.get()));
			
			System.out.println(Thread.currentThread().getId() + " " +"&&&&&&&&&&&& dim="+matched.size());
			//for(BasicPolicy printBp : matched) {
			//	printBp.write(Conf.policyDir + "rez"+this.getId());
				
			//}
			if (matched.size() > 0) {
				emptyCount.acquire();
				synchronized (partiallyMatched) {
					partiallyMatched.addAll(matched);
				}
				fillCount.release();
			}
		}
		
		public void run() {
			while(true) {
				try{
					sleep(delay);
					work();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
		
	}
	
	private boolean evalBoolean(String expr) {
		StringTokenizer st = new StringTokenizer(expr, " "+Conf.opConstants);
		String token1 = st.nextToken();
		String token2 = st.nextToken();
		
		if(expr.contains(">="))
			return (Long.parseLong(token1) >= Long.parseLong(token2));
		else if(expr.contains("<="))
			return (Long.parseLong(token1) <= Long.parseLong(token2));
		else if(expr.contains(">"))
			return (Long.parseLong(token1) > Long.parseLong(token2));
		else if(expr.contains("<"))
			return (Long.parseLong(token1) < Long.parseLong(token2));
		else if(expr.contains("!="))
			return (Long.parseLong(token1) != Long.parseLong(token2));
		else if(expr.contains("="))
			return (Long.parseLong(token1) == Long.parseLong(token2));
		
		return false;
			
	}
	
	private class PolicyCheck extends Thread {
		
		// checks policies from working queue
		public void work() throws Exception {
			
			//System.out.println(Thread.currentThread().getId() + " " +"~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			//System.out.println(Thread.currentThread().getId() + " " +"work!!");
			//System.out.println(Thread.currentThread().getId() + " " +"~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
			
			BasicPolicy bp = null;
			
			fillCount.acquire();
			synchronized (partiallyMatched) {
				bp = partiallyMatched.remove();
			}
			emptyCount.release();
			
			
			if(checked(bp)) {
				// blacklist client
				Element clientid = (Element)bp.getDom().getElementsByTagName("clientID").item(0);
				System.out.println(Thread.currentThread().getId() + " " +"~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
				System.out.println("[" + System.currentTimeMillis() + "] "
							+ Thread.currentThread().getId() + " " +"blacklist "+ "[" + clientid.getAttribute("rvalue") + "]");
				System.out.println(Thread.currentThread().getId() + " " +"~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
				
				String cmd = "/home/acarpena/Work/notifyPManagerClient/notifyProviderManager.sh " + clientid.getAttribute("rvalue") ;	
				Runtime run = Runtime.getRuntime() ;
				Process pr = run.exec(cmd) ;	
			}
			
		}
			
		public void run() {
			while(true) {
				try {
					System.out.println(Thread.currentThread().getId() + " " +"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
					sleep(1000);
					work();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
	}
	
	/**
	 * Get *.ser files from specified directory and subdirectories and load them
	 * into memory.
	 * 
	 * @param directory
	 */
	private void readPolicies(String directory) {
		System.out.println(Thread.currentThread().getId() + " " +"dir=" + directory);
		FileInputStream fis = null;
		ObjectInputStream in = null;
		try {

			File dir = new File(directory);
			FilenameFilter filter = new FilenameFilter() {
				public boolean accept(File dir, String name) {
					return name.endsWith(".ser");
				}
			};

			// filter serialized policies
			String[] children = dir.list(filter);
			if (children == null) {
				throw new Exception(directory + " is not a directory");
			} else {
				for (int i = 0; i < children.length; i++) {
					String filename = directory + children[i];
					fis = new FileInputStream(filename);
					System.out.println(Thread.currentThread().getId() + " " +"filename=" + filename);
					in = new ObjectInputStream(fis);
					BasicPolicy bp = (BasicPolicy) in.readObject();
					policies.add(bp);
					in.close();
				}
			}

			// filter directories and search recursively
			FileFilter fileFilter = new FileFilter() {
				public boolean accept(File file) {
					return file.isDirectory();
				}
			};

			File[] files = dir.listFiles(fileFilter);
			for (File f : files) {
				this.readPolicies(f.getCanonicalPath());
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	public static void main(String[] args) {
		// arguments: path to policy matching configuration file, path to the database configuration file
		// start the algorithm
		DebugMatching m = new DebugMatching(args[0], args[1]);

		System.out.println(Thread.currentThread().getId() + " " +m.policyDelays.toString());

		m.applyPolicies();
	}
}
