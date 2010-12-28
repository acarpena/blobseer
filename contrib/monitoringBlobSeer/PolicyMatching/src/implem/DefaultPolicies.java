/**
* Generate default security policies for BlobSeer
*
* @author Cristina Basescu
* @date 04.2010
*/

package implem;

import interf.policy.BasicPolicy;

import java.util.LinkedList;

import util.conf.Conf;
import util.policy.HighPolicy;
import util.policy.LowPolicy;
import util.policy.MediumPolicy;
import util.policy.Operation;
import util.xml.*;

public class DefaultPolicies {
	LinkedList<BasicPolicy> policies = new LinkedList<BasicPolicy>();
	
	/** denial of service on metadata providers */ 
	private HighPolicy dosmp() throws Exception {
		HighPolicy dosmp = new HighPolicy("c");
		// metadata request
		MetadataNodeRequestSum mr = new MetadataNodeRequestSum("mr1");
		mr.addElem(new ClientID(null, "c", "c"));
		mr.addElem(new MetadataProvider("prov", null, "p"));
		mr.addElem(new Count("count", null, "c1"));
		mr.addElem(new FirstDate("fd", null, "0"));
		mr.addElem(new LastDate("ld", null, "0"));
		mr.addElem(new Distance("dist", 1230000));
		mr.addElem(new Threshold("t", "300000"));
		mr.addElem(new SupThreshold("st", "600000"));
		mr.addElem(new Count("nodes", "nc", null, "c2"));
		mr.addElem(new Threshold("nodes", "tn", "14000000"));
		mr.addElem(new SupThreshold("nodes", "stn", "28000000"));
		Continuous cont = new Continuous(null);
		cont.addRefEvent(new RefEvent(null, "count"));
		cont.addRefEvent(new RefEvent(null, "ld"));
		cont.addRefEvent(new RefEvent(null, "nc"));
		mr.addElem(cont);
		dosmp.addPrecond(mr);
		// check			
		Check check = new Check("c1");
		Or or = new Or(null);
		or.addElem(new Content(null, new Operation(dosmp.elementIds, "count", "t", Operation.GT)));
		or.addElem(new Content(null, new Operation(dosmp.elementIds, "nc", "tn", Operation.GT)));
		And and = new And(null);
		and.addElem(or);
		and.addElem(new Content(null, new Operation(dosmp.elementIds, 
				new Operation(dosmp.elementIds, "ld", "fd", Operation.MINUS), "dist", Operation.GT)));
		check.addElem(and);
		dosmp.addPrecond( check);
		// add actions
		// a1
		Action a1 = new Action(null);
		Constraints cons = new Constraints(null, null);
		Advices ad1 = new Advices(null, null);
		And and1 = new And(null);
		and1.addElem(new Content(null, new Operation(dosmp.elementIds, "TL", 70, Operation.GT, "TL")));
		and1.addElem(new Content(null, new Operation(dosmp.elementIds, "nc", "stn", Operation.GT)));
		and1.addElem(new Content(null, new Operation(dosmp.elementIds, "count", "st", Operation.GT)));
		cons.addAnd(and1);
		a1.setConstraints(cons);
		And and2 = new And(null);
		and2.addElem(new EnablePolicy(null, "10_8"));
		and2.addElem(new DecreaseTL(null, "medium"));
		ad1.addAnd(and2);
		a1.setAdvices(ad1);
		dosmp.addAction(a1);
		// a2
		Action a2 = new Action(null);
		Constraints cons2 = new Constraints(null, null);
		Advices ad2 = new Advices(null, null);
		And and3 = new And(null);
		cons2.addConstraint(new Content(null, new Operation(dosmp.elementIds, "TL", 70, Operation.GT, "TL")));
		a2.setConstraints(cons2);
		and3.addElem(new EnablePolicy(null, "10_8"));
		and3.addElem(new DecreaseTL(null, "20"));
		ad2.addAnd(and3);
		a2.setAdvices(ad2);
		dosmp.addAction(a2);
		// a3
		Action a3 = new Action(null);
		Constraints cons3 = new Constraints(null, null);
		Advices ad3 = new Advices(null, null);
		And and4 = new And(null);
		and4.addElem(new Content(null, new Operation(dosmp.elementIds, "TL", 70, Operation.LT, "TL")));
		and4.addElem(new Content(null, new Operation(dosmp.elementIds, "TL", 30, Operation.GT, "TL")));
		cons3.addAnd(and4);
		a3.setConstraints(cons3);
		And and5 = new And(null);
		and5.addElem(new EnablePolicy(null, "20_9"));
		and5.addElem(new DecreaseTL(null, "low"));
		ad3.addAnd(and5);
		a3.setAdvices(ad3);
		dosmp.addAction(a3);
		// a4
		Action a4 = new Action(null);
		Constraints cons4 = new Constraints(null, null);
		Advices ad4 = new Advices(null, null);
		cons4.addConstraint(new Content(null, new Operation(dosmp.elementIds, "TL", 30, Operation.LT, "TL")));
		a4.setConstraints(cons4);
		ad4.addAdvice(new Blacklist(null, "c", "c"));
		a4.setAdvices(ad4);
		dosmp.addAction(a4);
		
		dosmp.setStart("mr1");
		dosmp.setEnd("c1");
		
		return dosmp;
	}
	
	/** WriteSum no publish */
	private HighPolicy wnp() throws Exception {
		HighPolicy wnp = new HighPolicy("c");
		
		WriteSum w1 = new WriteSum("w1");
		w1.addElem(new Watermark(null, "w", "w"));
		w1.addElem(new BlobID(null, "b", "b"));
		w1.addElem(new ClientID(null, "c", "c"));
		w1.addElem(new Count(null, null, "0"));
		w1.addElem(new LastDate("ld", null, "0"));
		Neg neg11 = new Neg(null);
		PrecededBy pb11 = new PrecededBy(null);
		pb11.addRefEvent(new RefEvent(null, "p1"));
		pb11.addCount(new Count(null, "1", null));
		pb11.addDistance(new Distance(null, new Operation(null, null, 25000, Operation.LT)));
		neg11.addElem(pb11);
		Neg neg21 = new Neg(null);
		FollowedBy fb11 = new FollowedBy(null);
		fb11.addRefEvent(new RefEvent(null, "p2"));
		fb11.addCount(new Count(null, "1", null));
		fb11.addDistance(new Distance(null, new Operation(null, null, 300000, Operation.LT)));
		neg21.addElem(fb11);
		w1.addElem(neg11);
		w1.addElem(neg21);
		wnp.addPrecond(w1);
		
		Publish p1 = new Publish("p1");
		p1.addElem(new Watermark(null, "w", "w"));
		p1.addElem(new BlobID(null, "b", "b"));
		p1.addElem(new ClientID(null, "c", "c"));
		p1.addElem(new Count(null, null, "0"));
		p1.addElem(new LastDate("ld", "d1", "d1"));
		wnp.addPrecond(p1);
		
		Publish p2 = new Publish("p2");
		p2.addElem(new Watermark(null, "w", "w"));
		p2.addElem(new BlobID(null, "b", "b"));
		p2.addElem(new ClientID(null, "c", "c"));
		p2.addElem(new Count(null, null, "0"));
		p2.addElem(new LastDate("ld", "d2", "d2"));
		wnp.addPrecond(p2);
		
		
		// add actions
		// a1
		Action a1 = new Action(null);
		Constraints cons = new Constraints(null, null);
		Advices ad1 = new Advices(null, null);
		cons.addConstraint(new Content(null, new Operation(wnp.elementIds, "TL", 70, Operation.GT, "TL")));
		a1.setConstraints(cons);
		ad1.addAdvice(new Log(null, "wnp for client c at timestamp d2"));
		a1.setAdvices(ad1);
		wnp.addAction(a1);
		
		// a3
		Action a3 = new Action(null);
		Constraints cons3 = new Constraints(null, null);
		Advices ad3 = new Advices(null, null);
		And and4 = new And(null);
		and4.addElem(new Content(null, new Operation(wnp.elementIds, "TL", 70, Operation.LT, "TL")));
		and4.addElem(new Content(null, new Operation(wnp.elementIds, "TL", 30, Operation.GT, "TL")));
		cons3.addAnd(and4);
		a3.setConstraints(cons3);
		And and5 = new And(null);
		and5.addElem(new EnablePolicy(null, "20_9"));
		and5.addElem(new DecreaseTL(null, "20"));
		ad3.addAnd(and5);
		a3.setAdvices(ad3);
		wnp.addAction(a3);
		// a4
		Action a4 = new Action(null);
		Constraints cons4 = new Constraints(null, null);
		Advices ad4 = new Advices(null, null);
		cons4.addConstraint(new Content(null, new Operation(wnp.elementIds, "TL", 30, Operation.LT, "TL")));
		a4.setConstraints(cons4);
		ad4.addAdvice(new Blacklist(null, "c", "c"));
		a4.setAdvices(ad4);
		wnp.addAction(a4);
		
		wnp.setStart("w1");
		wnp.setEnd("w1");
		
		return wnp;
	}
	
	/** publish no WriteSum */
	private HighPolicy pnw() throws Exception {
		HighPolicy pnw = new HighPolicy("c");
		
		Publish p1 = new Publish("p1");
		p1.addElem(new Watermark(null, "w", "w"));
		p1.addElem(new BlobID(null, "b", "b"));
		p1.addElem(new ClientID(null, "c", "c"));
		p1.addElem(new Count(null, null, "0"));
		p1.addElem(new LastDate("ldp1", "d1", "d1"));
		And and11 = new And(null);
		Neg neg11 = new Neg(null);
		PrecededBy pb11 = new PrecededBy(null);
		pb11.addRefEvent(new RefEvent(null, "w1"));
		Operation o = new Operation(pnw.elementIds, null, 0, Operation.NEQ);
		pb11.addCount(new Count(null, o.toString(), null));
		pb11.addDistance(new Distance(null, new Operation(pnw.elementIds, null, 300000, Operation.LTE)));
		neg11.addElem(pb11);
		Neg neg12 = new Neg(null);
		FollowedBy fb11 = new FollowedBy(null);
		fb11.addRefEvent(new RefEvent(null, "w2"));
		fb11.addCount(new Count(null, o.toString(), null));
		fb11.addDistance(new Distance(null, new Operation(pnw.elementIds, null, 25000, Operation.LTE)));
		neg12.addElem(fb11);
		and11.addElem(neg11);
		and11.addElem(neg12);
		p1.addElem(and11);
		pnw.addPrecond(p1);
		
		WriteSum w1 = new WriteSum("w1");
		w1.addElem(new Watermark(null, "w", "w"));
		w1.addElem(new BlobID(null, "b", "b"));
		w1.addElem(new ClientID(null, "c", "c"));
		w1.addElem(new Count(null, null, "0"));
		w1.addElem(new LastDate(null, null, "0"));
		Neg neg21 = new Neg(null);
		PrecededBy pb21 = new PrecededBy(null);
		pb21.addRefEvent(new RefEvent(null, "p2"));
		pb21.addCount(new Count(null, "1", null));
		pb21.addDistance(new Distance(null, new Operation(pnw.elementIds, null, 25000, Operation.LTE)));
		neg21.addElem(pb21);
		Neg neg22 = new Neg(null);
		FollowedBy fb21 = new FollowedBy(null);
		fb21.addRefEvent(new RefEvent(null, "p3"));
		fb21.addCount(new Count(null, "1", null));
		fb21.addDistance(new Distance(null, new Operation(pnw.elementIds, null, 300000, Operation.LTE)));
		neg22.addElem(fb21);
		w1.addElem(neg21);
		w1.addElem(neg22);
		pnw.addPrecond(w1);
		
		
		Publish p2 = new Publish("p2");
		p2.addElem(new Watermark(null, "w", "w"));
		p2.addElem(new BlobID(null, "b", "b"));
		p2.addElem(new ClientID(null, "c", "c"));
		p2.addElem(new Count(null, null, "0"));
		p2.addElem(new LastDate(null, null, "d2"));
		pnw.addPrecond(p2);
		
		Publish p3 = new Publish("p3");
		p3.addElem(new Watermark(null, "w", "w"));
		p3.addElem(new BlobID(null, "b", "b"));
		p3.addElem(new ClientID(null, "c", "c"));
		p3.addElem(new Count(null, null, "0"));
		p3.addElem(new LastDate("ldp3", null, "d3"));
		Check c1 = new Check("c1");
		c1.addElem(new Content(null, new Operation(pnw.elementIds, "ldp3", "ldp1", Operation.LT, "ldp3")));
		p3.addElem(c1);
		pnw.addPrecond(p3);
		
		WriteSum w2 = new WriteSum("w2");
		w2.addElem(new Watermark(null, "w", "w"));
		w2.addElem(new BlobID(null, "b", "b"));
		w2.addElem(new ClientID(null, "c", "c"));
		w2.addElem(new Count(null, null, "0"));
		w2.addElem(new LastDate(null, null, "0"));
		Neg neg31 = new Neg(null);
		PrecededBy pb31 = new PrecededBy(null);
		pb31.addRefEvent(new RefEvent(null, "p4"));
		pb31.addCount(new Count(null, "1", null));
		pb31.addDistance(new Distance(null, new Operation(pnw.elementIds, null, 25000, Operation.LTE)));
		neg31.addElem(pb31);
		w2.addElem(neg31);
		pnw.addPrecond(w2);
		
		Publish p4 = new Publish("p4");
		p4.addElem(new Watermark(null, "w", "w"));
		p4.addElem(new BlobID(null, "b", "b"));
		p4.addElem(new ClientID(null, "c", "c"));
		p4.addElem(new Count(null, null, "0"));
		p4.addElem(new LastDate("ldp4", null, "d4"));
		Check c2 = new Check("c2");
		c2.addElem(new Content(null, new Operation(pnw.elementIds, "ldp4", "ldp1", Operation.LT, "ldp4")));
		p4.addElem(c2);
		pnw.addPrecond(p4);
		
		// add actions
		// a1
		Action a1 = new Action(null);
		Constraints cons = new Constraints(null, null);
		Advices ad1 = new Advices(null, null);
		a1.setConstraints(cons);
		ad1.addAdvice(new Blacklist(null, "c", "c"));
		a1.setAdvices(ad1);
		pnw.addAction(a1);
		
		pnw.setStart("p1");
		pnw.setEnd("p1");
		
		return pnw;
	}
	
	/** denial of service on reads */
	private HighPolicy dosr() throws Exception {
		HighPolicy dosr = new HighPolicy("c");
		
		PageRequest pr = new PageRequest("pr1");
		pr.addElem(new PageID("page", null, "p"));
		pr.addElem(new ClientID(null, "c", "c"));
		pr.addElem(new Count("readSize", "rsc", null, "rs1"));
		pr.addElem(new Threshold("readSize", "trs", "1234000"));
		pr.addElem(new SupThreshold("readSize", "strs", "5678000"));
		pr.addElem(new FirstDate("fd", null, "0"));
		pr.addElem(new LastDate("ld", null, "0"));
		pr.addElem(new Distance("dist", 1230000));
		Continuous cont = new Continuous(null);
		cont.addRefEvent(new RefEvent(null, "page"));
		cont.addRefEvent(new RefEvent(null, "rsc"));
		cont.addRefEvent(new RefEvent(null, "ld"));
		pr.addElem(cont);
		dosr.addPrecond(pr);
			
		Check check = new Check("c1");
		And and = new And(null);
		and.addElem(new Content(null, new Operation(dosr.elementIds, "rsc", "trs", Operation.GT)));
		and.addElem(new Content(null, new Operation(dosr.elementIds, 
				new Operation(dosr.elementIds, "ld", "fd", Operation.MINUS), "dist", Operation.GT)));
		check.addElem(and);
		dosr.addPrecond(check);
		
		// add actions
		// a1
		Action a1 = new Action(null);
		Constraints cons = new Constraints(null, null);
		Advices ad1 = new Advices(null, null);
		And and1 = new And(null);
		and1.addElem(new Content(null, new Operation(dosr.elementIds, "TL", 70, Operation.GT, "TL")));
		and1.addElem(new Content(null, new Operation(dosr.elementIds, "rsc", "strs", Operation.GT)));
		cons.addAnd(and1);
		a1.setConstraints(cons);
		And and2 = new And(null);
		and2.addElem(new EnablePolicy(null, "10_8"));
		and2.addElem(new DecreaseTL(null, "medium"));
		ad1.addAnd(and2);
		a1.setAdvices(ad1);
		dosr.addAction(a1);
		// a2
		Action a2 = new Action(null);
		Constraints cons2 = new Constraints(null, null);
		Advices ad2 = new Advices(null, null);
		And and3 = new And(null);
		cons2.addConstraint(new Content(null, new Operation(dosr.elementIds, "TL", 70, Operation.GT, "TL")));
		a2.setConstraints(cons2);
		and3.addElem(new EnablePolicy(null, "10_8"));
		and3.addElem(new DecreaseTL(null, "20"));
		ad2.addAnd(and3);
		a2.setAdvices(ad2);
		dosr.addAction(a2);
		// a3
		Action a3 = new Action(null);
		Constraints cons3 = new Constraints(null, null);
		Advices ad3 = new Advices(null, null);
		And and4 = new And(null);
		and4.addElem(new Content(null, new Operation(dosr.elementIds, "TL", 70, Operation.LT, "TL")));
		and4.addElem(new Content(null, new Operation(dosr.elementIds, "TL", 30, Operation.GT, "TL")));
		cons3.addAnd(and4);
		a3.setConstraints(cons3);
		And and5 = new And(null);
		and5.addElem(new EnablePolicy(null, "20_9"));
		and5.addElem(new DecreaseTL(null, "low"));
		ad3.addAnd(and5);
		a3.setAdvices(ad3);
		dosr.addAction(a3);
		// a4
		Action a4 = new Action(null);
		Constraints cons4 = new Constraints(null, null);
		Advices ad4 = new Advices(null, null);
		cons4.addConstraint(new Content(null, new Operation(dosr.elementIds, "TL", 30, Operation.LT, "TL")));
		a4.setConstraints(cons4);
		ad4.addAdvice(new Blacklist(null, "c", "c"));
		a4.setAdvices(ad4);
		dosr.addAction(a4);
		
		dosr.setStart("pr1");
		dosr.setEnd("c1");
		
		return dosr;
	}
	
	/** denial of service on provider manager */
	private HighPolicy dospm() throws Exception {
		HighPolicy dospm = new HighPolicy("c");
		
		ProviderManagerRequestSum pmr = new ProviderManagerRequestSum("pmr1");
		pmr.addElem(new ClientID(null, "c", "c"));
		pmr.addElem(new Count("count", null, "co1"));
		pmr.addElem(new FirstDate("fd", null, "0"));
		pmr.addElem(new LastDate("ld", null, "0"));
		pmr.addElem(new Distance("dist", 1230000000));
		pmr.addElem(new Threshold("t", "300000"));
		pmr.addElem(new SupThreshold("st", "600000"));
		pmr.addElem(new Count("providers", "pc", null, "c2"));
		pmr.addElem(new Threshold("providers", "tp", "14000000"));
		pmr.addElem(new SupThreshold("providers", "stp", "28000000"));
		Continuous cont = new Continuous(null);
		cont.addRefEvent(new RefEvent(null, "count"));
		cont.addRefEvent(new RefEvent(null, "ld"));
		cont.addRefEvent(new RefEvent(null, "nc"));
		pmr.addElem(cont);
		dospm.addPrecond(pmr);
			
		Check check = new Check("c1");
		Or or = new Or(null);
		or.addElem(new Content(null, new Operation(dospm.elementIds, "count", "t", Operation.GT)));
		or.addElem(new Content(null, new Operation(dospm.elementIds, "pc", "tp", Operation.GT)));
		And and = new And(null);
		and.addElem(or);
		and.addElem(new Content(null, new Operation(dospm.elementIds, 
				new Operation(dospm.elementIds, "ld", "fd", Operation.MINUS), "dist", Operation.GT)));
		check.addElem(and);
		dospm.addPrecond(check);
		
		// add actions
		// a1
		Action a1 = new Action(null);
		Constraints cons = new Constraints(null, null);
		Advices ad1 = new Advices(null, null);
		And and1 = new And(null);
		and1.addElem(new Content(null, new Operation(dospm.elementIds, "TL", 70, Operation.GT, "TL")));
		and1.addElem(new Content(null, new Operation(dospm.elementIds, "pc", "stp", Operation.GT)));
		and1.addElem(new Content(null, new Operation(dospm.elementIds, "count", "st", Operation.GT)));
		cons.addAnd(and1);
		a1.setConstraints(cons);
		And and2 = new And(null);
		and2.addElem(new EnablePolicy(null, "10_8"));
		and2.addElem(new DecreaseTL(null, "medium"));
		ad1.addAnd(and2);
		a1.setAdvices(ad1);
		dospm.addAction(a1);
		// a2
		Action a2 = new Action(null);
		Constraints cons2 = new Constraints(null, null);
		Advices ad2 = new Advices(null, null);
		And and3 = new And(null);
		cons2.addConstraint(new Content(null, new Operation(dospm.elementIds, "TL", 70, Operation.GT, "TL")));
		a2.setConstraints(cons2);
		and3.addElem(new EnablePolicy(null, "10_8"));
		and3.addElem(new DecreaseTL(null, "20"));
		ad2.addAnd(and3);
		a2.setAdvices(ad2);
		dospm.addAction(a2);
		// a3
		Action a3 = new Action(null);
		Constraints cons3 = new Constraints(null, null);
		Advices ad3 = new Advices(null, null);
		And and4 = new And(null);
		and4.addElem(new Content(null, new Operation(dospm.elementIds, "TL", 70, Operation.LT, "TL")));
		and4.addElem(new Content(null, new Operation(dospm.elementIds, "TL", 30, Operation.GT, "TL")));
		cons3.addAnd(and4);
		a3.setConstraints(cons3);
		And and5 = new And(null);
		and5.addElem(new EnablePolicy(null, "20_9"));
		and5.addElem(new DecreaseTL(null, "low"));
		ad3.addAnd(and5);
		a3.setAdvices(ad3);
		dospm.addAction(a3);
		// a4
		Action a4 = new Action(null);
		Constraints cons4 = new Constraints(null, null);
		Advices ad4 = new Advices(null, null);
		cons4.addConstraint(new Content(null, new Operation(dospm.elementIds, "TL", 30, Operation.LT, "TL")));
		a4.setConstraints(cons4);
		ad4.addAdvice(new Blacklist(null, "c", "c"));
		a4.setAdvices(ad4);
		dospm.addAction(a4);
		
		dospm.setStart("pmr1");
		dospm.setEnd("c1");
		
		return dospm;
	}
	
	/** denial of service on WriteSums */
	private HighPolicy dosw() throws Exception {
		HighPolicy dosw = new HighPolicy("c");
		
		WriteSum w1 = new WriteSum("w1");
		w1.addElem(new Watermark("wa", "w", ""));
		w1.addElem(new BlobID("bId", "b", ""));
		w1.addElem(new ClientID(null, "c", ""));
		w1.addElem(new Count("WriteSize", "wsc", null, ""));
		//w1.addElem(new WrittenPages("wp", "500000"));
		w1.addElem(new Threshold("WriteSize", "tws", "1080"));
		w1.addElem(new SupThreshold("WriteSize", "stws", "2000"));
		w1.addElem(new FirstDate("fd", null, ""));
		w1.addElem(new LastDate("ld", null, ""));
		w1.addElem(new Distance("dist", 7000));
		Continuous cont = new Continuous(null);
		cont.addRefEvent(new RefEvent(null, "wa"));
		cont.addRefEvent(new RefEvent(null, "bId"));
		cont.addRefEvent(new RefEvent(null, "wsc"));
		cont.addRefEvent(new RefEvent(null, "ld"));
		w1.addElem(cont);
		Neg neg11 = new Neg(null);
		PrecededBy pb11 = new PrecededBy(null);
		pb11.addRefEvent(new RefEvent(null, "p1"));
		pb11.addCount(new Count(null, "1", null));
		pb11.addDistance(new Distance(null, new Operation(null, null, 
				new Operation(null, "fd", 100, Operation.MINUS, "fd"), Operation.GT)));
		neg11.addElem(pb11);
		Neg neg21 = new Neg(null);
		FollowedBy fb11 = new FollowedBy(null);
		fb11.addRefEvent(new RefEvent(null, "p2"));
		fb11.addCount(new Count(null, "1", null));
		fb11.addDistance(new Distance(null, new Operation(null, null, 
				new Operation(null, "fd", "dist", Operation.PLUS, "fd", "dist"), Operation.LTE)));
		neg21.addElem(fb11);
		w1.addElem(neg11);
		w1.addElem(neg21);
		dosw.addPrecond(w1);
		
		Publish p1 = new Publish("p1");
		p1.addElem(new Watermark(null, "w", ""));
		p1.addElem(new BlobID(null, "b", ""));
		p1.addElem(new ClientID(null, "c", ""));
		p1.addElem(new Count(null, null, "0"));
		p1.addElem(new LastDate(null, null, ""));
		dosw.addPrecond(p1);
		
		Publish p2 = new Publish("p2");
		p2.addElem(new Watermark(null, "w", ""));
		p2.addElem(new BlobID(null, "b", ""));
		p2.addElem(new ClientID(null, "c", ""));
		p2.addElem(new Count(null, null, "0"));
		p2.addElem(new LastDate(null, null, ""));
		dosw.addPrecond(p2);
			
		Check check = new Check("c1");
		check.addElem(new Content(null, new Operation(dosw.elementIds, "wsc", "tws", Operation.GT)));
		dosw.addPrecond(check);
		
		// add actions
		// a1
		Action a1 = new Action(null);
		Constraints cons = new Constraints(null, null);
		Advices ad1 = new Advices(null, null);
		And and1 = new And(null);
		and1.addElem(new Content(null, new Operation(dosw.elementIds, "TL", 70, Operation.GT, "TL")));
		and1.addElem(new Content(null, new Operation(dosw.elementIds, "wsc", "stws", Operation.GT)));
		cons.addAnd(and1);
		a1.setConstraints(cons);
		And and2 = new And(null);
		and2.addElem(new EnablePolicy(null, "10_8"));
		and2.addElem(new DecreaseTL(null, "medium"));
		ad1.addAnd(and2);
		a1.setAdvices(ad1);
		dosw.addAction(a1);
		// a2
		Action a2 = new Action(null);
		Constraints cons2 = new Constraints(null, null);
		Advices ad2 = new Advices(null, null);
		And and3 = new And(null);
		cons2.addConstraint(new Content(null, new Operation(dosw.elementIds, "TL", 70, Operation.GT, "TL")));
		a2.setConstraints(cons2);
		and3.addElem(new EnablePolicy(null, "10_1"));
		and3.addElem(new DecreaseTL(null, "20"));
		ad2.addAnd(and3);
		a2.setAdvices(ad2);
		dosw.addAction(a2);
		// a3
		Action a3 = new Action(null);
		Constraints cons3 = new Constraints(null, null);
		Advices ad3 = new Advices(null, null);
		And and4 = new And(null);
		and4.addElem(new Content(null, new Operation(dosw.elementIds, "TL", 70, Operation.LT, "TL")));
		and4.addElem(new Content(null, new Operation(dosw.elementIds, "TL", 30, Operation.GT, "TL")));
		cons3.addAnd(and4);
		a3.setConstraints(cons3);
		And and5 = new And(null);
		and5.addElem(new EnablePolicy(null, "20_9"));
		and5.addElem(new DecreaseTL(null, "low"));
		ad3.addAnd(and5);
		a3.setAdvices(ad3);
		dosw.addAction(a3);
		// a4
		Action a4 = new Action(null);
		Constraints cons4 = new Constraints(null, null);
		Advices ad4 = new Advices(null, null);
		cons4.addConstraint(new Content(null, new Operation(dosw.elementIds, "TL", 30, Operation.LT, "TL")));
		a4.setConstraints(cons4);
		ad4.addAdvice(new Blacklist(null, "c", "c"));
		a4.setAdvices(ad4);
		dosw.addAction(a4);
		
		dosw.setStart("w1");
		dosw.setEnd("c1");
		
		return dosw;
	}
	
	/** read pages without having requested metadata for them */
	private HighPolicy rnm() throws Exception {
		HighPolicy rnm = new HighPolicy("c");
		
		PageRequest pr = new PageRequest("pr1");
		pr.addElem(new PageID(null, "p", "p"));
		pr.addElem(new ClientID(null, "c", "c"));
		pr.addElem(new LastDate(null, null, "d1"));
		Neg neg11 = new Neg(null);
		PrecededBy pb11 = new PrecededBy(null);
		pb11.addRefEvent(new RefEvent(null, "mr1"));
		Operation o = new Operation(null, null, 0, Operation.NEQ);
		pb11.addCount(new Count(null, o.toString(), null));
		pb11.addDistance(new Distance(null));
		neg11.addElem(pb11);
		pr.addElem(neg11);
		rnm.addPrecond(pr);
		
		MetadataPageRequest mr = new MetadataPageRequest("mr1");
		mr.addElem(new PageID(null, "p", "p"));
		mr.addElem(new ClientID(null, "c", "c"));
		mr.addElem(new Count(null, null, "0"));
		mr.addElem(new LastDate(null, null, "d2"));
		rnm.addPrecond(mr);
		
		// add actions
		// a1
		Action a1 = new Action(null);
		Constraints cons = new Constraints(null, null);
		Advices ad1 = new Advices(null, null);
		a1.setConstraints(cons);
		ad1.addAdvice(new Blacklist(null, "c", "c"));
		a1.setAdvices(ad1);
		rnm.addAction(a1);
		
		rnm.setStart("pr1");
		rnm.setEnd("pr1");
		
		return rnm;
	}
	
	/** crawling */
	private MediumPolicy crw() throws Exception {
		MediumPolicy crw = new MediumPolicy("c");
		
		EModule mod = new EModule(null, "permission");
		Query q = new Query(null, "read");
		q.addElem(new ClientID(null, null, "c"));
		Answer a = new Answer(null, null);
		a.addElem(new BlobID("bids", null, "list of blob ids"));
		a.addElem(new Count("dataSize", "dsc", null, "0"));
		mod.setQuery(q);
		mod.setAnswer(a);
		crw.addEData(mod);
		
		PageRequest pr = new PageRequest("pr1");
		pr.addElem(new PageID("page", null, "p"));
		pr.addElem(new BlobID("bid", null, "b"));
		pr.addElem(new ClientID(null, null, "c"));
		pr.addElem(new Count("readSize", "rsc", null, "rs1"));
		pr.addElem(new Threshold("percentRead", "tpr", "90"));
		pr.addElem(new FirstDate("fd", null, "0"));
		pr.addElem(new LastDate("ld", null, "0"));
		pr.addElem(new Distance("dist", 1230000));
		Check check = new Check("ch1");
		check.addElem(new Content(null, new Operation(crw.elementIds, "bid", "bids", Operation.SUBSET, "bid")));
		pr.addElem(check);
		Continuous cont = new Continuous(null);
		cont.addRefEvent(new RefEvent(null, "page"));
		cont.addRefEvent(new RefEvent(null, "bid"));
		cont.addRefEvent(new RefEvent(null, "rsc"));
		cont.addRefEvent(new RefEvent(null, "ld"));
		pr.addElem(cont);
		crw.addPrecond(pr);
		
		Check ch2 = new Check("ch2");
		And and = new And(null);
		and.addElem(new Content(null, new Operation(crw.elementIds, 
			new Operation(crw.elementIds, 
			new Operation(crw.elementIds, "rsc", "dsc", Operation.DIVISION), 100, Operation.MULTIPLY),
			"tpr", Operation.GT)));
		and.addElem(new Content(null, new Operation(crw.elementIds, 
			new Operation(crw.elementIds, "ld", "fd", Operation.MINUS), "dist", Operation.GT)));
		ch2.addElem(and);
		crw.addPrecond(ch2);
		
		// a1
		Action a1 = new Action(null);
		Constraints cons1 = new Constraints(null, null);
		Advices ad1 = new Advices(null, null);
		a1.setConstraints(cons1);
		ad1.addAdvice(new DecreaseTL(null, "10"));
		a1.setAdvices(ad1);
		crw.addAction(a1);
		
		crw.setStart("pr1");
		crw.setEnd("ch2");
		
		return crw;
	}
	
	/** intensive data overwriting */
	private LowPolicy ido() throws Exception {
		LowPolicy ido = new LowPolicy("c");
		
		Publish p1 = new Publish("p1");
		p1.addElem(new BlobID(null, "b", "rb"));
		p1.addElem(new Offset(null, "o", "ro"));
		p1.addElem(new ClientID(null, "c", "c"));
		p1.addElem(new Count("count", null, "0"));
		p1.addElem(new Threshold("count", "tc", "5"));
		p1.addElem(new SupThreshold("count", "stc", "100"));
		p1.addElem(new FirstDate("fd", null, "0"));
		p1.addElem(new LastDate("ld", null, "0"));
		p1.addElem(new Distance("dist", 1230000000));
		Continuous cont = new Continuous(null);
		cont.addRefEvent(new RefEvent(null, "count"));
		p1.addElem(cont);
		ido.addPrecond(p1);
		// check			
		Check check = new Check("c1");
		And and = new And(null);
		and.addElem(new Content(null, new Operation(ido.elementIds, "count", "tc", Operation.GT)));
		and.addElem(new Content(null, new Operation(ido.elementIds, 
				new Operation(ido.elementIds, "ld", "fd", Operation.MINUS), "dist", Operation.GT)));
		check.addElem(and);
		ido.addPrecond(check);
		// add actions
		// a1
		Action a1 = new Action(null);
		Constraints cons = new Constraints(null, null);
		Advices ad1 = new Advices(null, null);
		cons.addConstraint(new Content(null, new Operation(ido.elementIds, "count", "stc", Operation.GT)));
		a1.setConstraints(cons);
		ad1.addAdvice(new DecreaseTL(null, "20"));
		a1.setAdvices(ad1);
		ido.addAction(a1);
		// a2
		Action a2 = new Action(null);
		Constraints cons2 = new Constraints(null, null);
		Advices ad2 = new Advices(null, null);
		a2.setConstraints(cons2);
		ad2.addAdvice(new DecreaseTL(null, "20"));
		a2.setAdvices(ad2);
		ido.addAction(a2);
		
		ido.setStart("p1");
		ido.setEnd("c1");
		
		return ido;
	}
	
	/** incorrect WriteSums */
	private HighPolicy iw() throws Exception {
		HighPolicy iw = new HighPolicy("c");
		
		Publish p1 = new Publish("p1");
		p1.addElem(new Watermark(null, "w", "w"));
		p1.addElem(new BlobID(null, "b", "b"));
		p1.addElem(new ClientID(null, "c", "c"));
		p1.addElem(new LastDate("ldp1", null, "d1"));
		p1.addElem(new WriteSize("wsizep", null, "0"));
		PrecededBy pb11 = new PrecededBy(null);
		pb11.addRefEvent(new RefEvent(null, "w1"));
		pb11.addDistance(new Distance(null, new Operation(null, null, 300000, Operation.LT)));
		FollowedBy fb11 = new FollowedBy(null);
		fb11.addRefEvent(new RefEvent(null, "w2"));
		fb11.addDistance(new Distance(null, new Operation(null, null, 25000, Operation.LT)));
		p1.addElem(pb11);
		p1.addElem(fb11);
		iw.addPrecond(p1);
		
		WriteSum w1 = new WriteSum("w1");
		w1.addElem(new Watermark(null, "w", "w"));
		w1.addElem(new BlobID(null, "b", "b"));
		w1.addElem(new ClientID(null, "c", "c"));
		w1.addElem(new Count("countw1", null, "0"));
		w1.addElem(new FirstDate("fdw1", null, "0"));
		w1.addElem(new LastDate("ldw1", null, "0"));
		w1.addElem(new WriteSize("wsizew1", null, "ws1"));
		Continuous cont = new Continuous(null);
		cont.addRefEvent(new RefEvent(null, "countw1"));
		cont.addRefEvent(new RefEvent(null, "fdw1"));
		cont.addRefEvent(new RefEvent(null, "wsizew1"));
		w1.addElem(cont);
		Neg neg11 = new Neg(null);
		PrecededBy pb21 = new PrecededBy(null);
		pb21.addRefEvent(new RefEvent(null, "p2"));
		pb21.addCount(new Count(null, "1", null));
		pb21.addDistance(new Distance(null, new Operation(null, null, 25000, Operation.LT)));
		neg11.addElem(pb21);
		Neg neg21 = new Neg(null);
		FollowedBy fb21 = new FollowedBy(null);
		fb21.addRefEvent(new RefEvent(null, "p3"));
		fb21.addCount(new Count(null, "1", null));
		fb21.addDistance(new Distance(null, new Operation(null, null, 300000, Operation.LT)));
		neg21.addElem(fb21);
		w1.addElem(neg11);
		w1.addElem(neg21);
		iw.addPrecond(w1);
		
		WriteSum w2 = new WriteSum("w2");
		w2.addElem(new Watermark(null, "w", "w"));
		w2.addElem(new BlobID(null, "b", "b"));
		w2.addElem(new ClientID(null, "c", "c"));
		w2.addElem(new Count("countw2", null, "0"));
		w2.addElem(new FirstDate("fdw2", null, "0"));
		w2.addElem(new LastDate("ldw2", null, "0"));
		w2.addElem(new WriteSize("wsizew2", null, "ws2"));
		Continuous cont2 = new Continuous(null);
		cont2.addRefEvent(new RefEvent(null, "countw2"));
		cont2.addRefEvent(new RefEvent(null, "ldw2"));
		cont2.addRefEvent(new RefEvent(null, "wsizew2"));
		w2.addElem(cont2);
		Neg neg31 = new Neg(null);
		PrecededBy pb31 = new PrecededBy(null);
		pb31.addRefEvent(new RefEvent(null, "p4"));
		pb31.addCount(new Count(null, "1", null));
		pb31.addDistance(new Distance(null, new Operation(null, null, 25000, Operation.LTE)));
		neg31.addElem(pb31);
		w2.addElem(neg31);
		iw.addPrecond(w2);
		
		Publish p2 = new Publish("p2");
		p2.addElem(new Watermark(null, "w", "w"));
		p2.addElem(new BlobID(null, "b", "b"));
		p2.addElem(new ClientID(null, "c", "c"));
		p2.addElem(new Count(null, null, "0"));
		p2.addElem(new LastDate(null, null, "ldp2"));
		iw.addPrecond(p2);
		
		Publish p3 = new Publish("p3");
		p3.addElem(new Watermark(null, "w", "w"));
		p3.addElem(new BlobID(null, "b", "b"));
		p3.addElem(new ClientID(null, "c", "c"));
		p3.addElem(new Count(null, null, "0"));
		p3.addElem(new LastDate("ldp3", null, "ldp3"));
		Check ch = new Check("c1");
		ch.addElem(new Content(null, new Operation(iw.elementIds, "ldp3", "ldp1", Operation.LT, "ldp3")));
		p3.addElem(ch);
		iw.addPrecond(p3);
		
		Publish p4 = new Publish("p4");
		p4.addElem(new Watermark(null, "w", "w"));
		p4.addElem(new BlobID(null, "b", "b"));
		p4.addElem(new ClientID(null, "c", "c"));
		p4.addElem(new Count(null, null, "0"));
		p4.addElem(new LastDate("ldp4", null, "ldp4"));
		Check ch2 = new Check("c2");
		ch2.addElem(new Content(null, new Operation(iw.elementIds, "ldp4", "ldp1", Operation.LT, "ldp4")));
		p4.addElem(ch2);
		iw.addPrecond(p4);
		
		Check ch3 = new Check("c3");
		ch3.addElem(new Content(null, new Operation(iw.elementIds, "wsizep", 
			new Operation(iw.elementIds, "wsizew1", "wsizew2", Operation.PLUS), Operation.NEQ)));
		iw.addPrecond(ch3);
		
		// add actions
		// a1
		Action a1 = new Action(null);
		Constraints cons = new Constraints(null, null);
		Advices ad1 = new Advices(null, null);
		And and1 = new And(null);
		and1.addElem(new Content(null, new Operation(iw.elementIds, "wsizep", 
			new Operation(iw.elementIds, "wsizew1", "wsizew2", Operation.PLUS), Operation.GT)));
		and1.addElem(new Content(null, new Operation(iw.elementIds, "TL", 70, Operation.GT, "TL")));
		cons.addAnd(and1);
		a1.setConstraints(cons);
		ad1.addAdvice(new Log(null, "iw for client c with exceed on data providers"));
		a1.setAdvices(ad1);
		iw.addAction(a1);
		
		// a2
		Action a2 = new Action(null);
		Constraints cons2 = new Constraints(null, null);
		Advices ad2 = new Advices(null, null);
		And and2 = new And(null);
		and2.addElem(new Content(null, new Operation(iw.elementIds, "wsizep", 
			new Operation(iw.elementIds, "wsizew1", "wsizew2", Operation.PLUS), Operation.GT)));
		and2.addElem(new Content(null, new Operation(iw.elementIds, "TL", 70, Operation.LT, "TL")));
		and2.addElem(new Content(null, new Operation(iw.elementIds, "TL", 30, Operation.GT, "TL")));
		cons2.addAnd(and2);
		a2.setConstraints(cons2);
		And and3 = new And(null);
		and3.addElem(new EnablePolicy(null, "20_9"));
		and3.addElem(new DecreaseTL(null, "20"));
		ad2.addAnd(and3);
		a2.setAdvices(ad2);
		iw.addAction(a2);
		
		// a3
		Action a3 = new Action(null);
		Constraints cons3 = new Constraints(null, null);
		Advices ad3 = new Advices(null, null);
		Or or = new Or(null);
		or.addElem(new Content(null, new Operation(iw.elementIds, "wsizep", 
			new Operation(iw.elementIds, "wsizew1", "wsizew2", Operation.PLUS), Operation.LT)));
		And and4 = new And(null);
		and4.addElem(new Content(null, new Operation(iw.elementIds, "TL", 30, Operation.LT, "TL")));
		and4.addElem(new Content(null, new Operation(iw.elementIds, "wsizep", 
			new Operation(iw.elementIds, "wsizew1", "wsizew2", Operation.PLUS), Operation.GT)));
		or.addElem(and4);
		cons3.addOr(or);
		a3.setConstraints(cons3);
		ad3.addAdvice(new Blacklist(null, "c", "c"));
		a3.setAdvices(ad3);
		iw.addAction(a3);
		
		iw.setStart("p1");
		iw.setEnd("c3");
		
		return iw;
	}
	
	
	
	private HighPolicy DoSproviders() throws Exception {
		HighPolicy dosw = new HighPolicy("c");
		
		WriteSum w1 = new WriteSum("w1");
		w1.addElem(new Watermark("wa", "w", ""));
		w1.addElem(new BlobID("bId", "b", ""));
		w1.addElem(new ClientID(null, "c", ""));
		w1.addElem(new Count("Written_pages", "wsc", null, ""));
		//w1.addElem(new WrittenPages("wp", "500000"));
		w1.addElem(new Threshold("Written_pages", "tws", "2"));
		w1.addElem(new SupThreshold("Written_pages", "stws", "20"));
		w1.addElem(new FirstDate("fd", null, ""));
		w1.addElem(new LastDate("ld", null, ""));
		w1.addElem(new Distance("dist", 500));
		Continuous cont = new Continuous(null);
		cont.addRefEvent(new RefEvent(null, "wa"));
		cont.addRefEvent(new RefEvent(null, "bId"));
		cont.addRefEvent(new RefEvent(null, "wsc"));
		cont.addRefEvent(new RefEvent(null, "ld"));
		w1.addElem(cont);
		Neg neg11 = new Neg(null);
		PrecededBy pb11 = new PrecededBy(null);
		pb11.addRefEvent(new RefEvent(null, "p1"));
		pb11.addCount(new Count(null, "400", null));
		pb11.addDistance(new Distance(null, new Operation(null, null, 
				new Operation(null, "fd", 1, Operation.MINUS, "fd"), Operation.GT)));
		neg11.addElem(pb11);
		Neg neg21 = new Neg(null);
		FollowedBy fb11 = new FollowedBy(null);
		fb11.addRefEvent(new RefEvent(null, "p2"));
		fb11.addCount(new Count(null, "1", null));
		fb11.addDistance(new Distance(null, new Operation(null, null, 
				new Operation(null, "fd", "dist", Operation.PLUS, "fd", "dist"), Operation.LTE)));
		neg21.addElem(fb11);
		w1.addElem(neg11);
		w1.addElem(neg21);
		dosw.addPrecond(w1);
		
		Publish p1 = new Publish("p1");
		p1.addElem(new Watermark(null, "w", ""));
		p1.addElem(new BlobID(null, "b", ""));
		p1.addElem(new ClientID(null, "c", ""));
		p1.addElem(new Count(null, null, "0"));
		p1.addElem(new LastDate(null, null, ""));
		dosw.addPrecond(p1);
		
		Publish p2 = new Publish("p2");
		p2.addElem(new Watermark(null, "w", ""));
		p2.addElem(new BlobID(null, "b", ""));
		p2.addElem(new ClientID(null, "c", ""));
		p2.addElem(new Count(null, null, "0"));
		p2.addElem(new LastDate(null, null, ""));
		dosw.addPrecond(p2);
			
		Check check = new Check("c1");
		check.addElem(new Content(null, new Operation(dosw.elementIds, "wsc", "tws", Operation.GT)));
		dosw.addPrecond(check);
		
		// add actions
		// a4
		Action a4 = new Action(null);
		Constraints cons4 = new Constraints(null, null);
		Advices ad4 = new Advices(null, null);
		//cons4.addConstraint(new Content(null, new Operation(dosw.elementIds, "TL", 30, Operation.LT, "TL")));
		a4.setConstraints(cons4);
		ad4.addAdvice(new Blacklist(null, "c", "c"));
		a4.setAdvices(ad4);
		dosw.addAction(a4);
		
		dosw.setStart("w1");
		dosw.setEnd("c1");
		
		return dosw;
	}
	
	
	
	
	public DefaultPolicies(String matching_conf) {
		Conf.confFile = matching_conf;
		Conf.readConf();
		
		try{
			/*policies.add(dosmp());
			policies.add(wnp());
			policies.add(pnw());
			policies.add(dosr());
			policies.add(dospm());*/
			//policies.add(dosw());
			/*policies.add(rnm());
			policies.add(crw());
			policies.add(ido());
			policies.add(iw());*/
			
			policies.add(DoSproviders());
			
		}catch(Exception e){
			System.out.println(e.getMessage());
			e.printStackTrace();
		}
	}
	
	/**
	 * Builds XML policies from the policies list.
	 */
	public void buildPolicies() {
		for (BasicPolicy bp : policies) {
			try {
				bp.buildXML();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		Conf.writeConf();
	}
	
	/**
	 * Serialize XML policies.
	 */
	public void serializePolicies() {
		for (BasicPolicy bp : policies) {
			try {
				bp.serialize();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		Conf.writeConf();
	}
	
	public static void main(String[] args) {
		// build default policies
		// argument: the path to the configuration file
		DefaultPolicies dp = new DefaultPolicies(args[0]);
		dp.buildPolicies();
		dp.serializePolicies();
	}
}
