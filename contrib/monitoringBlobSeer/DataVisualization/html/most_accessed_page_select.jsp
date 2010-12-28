<%@ page import="lia.web.servlets.web.display" %>
<%@ page import="lia.web.utils.Page" %>
<%@ page import="lia.web.utils.Formatare" %>
<%@ page import="java.util.Date" %>
<%@ page import="java.util.StringTokenizer" %>
<%@ page import="java.io.*" %>
<%@ page import="lia.web.utils.DoubleFormat" %>
<%@ page import="lia.Monitor.Store.Cache" %>
<%@ page import="lia.Monitor.monitor.*" %>
<%@ page import="lia.Monitor.Store.Fast.DB" %>
<%@ page import="lia.Monitor.Store.Cache" %>
<%@ page import="lia.Monitor.Store.FarmBan" %>
<%@ page import="lia.web.utils.Formatare" %>
<%@ page import="lia.Monitor.monitor.monPredicate" %>
<%
    ServletContext sc = getServletContext();
    final String SITE_BASE = sc.getRealPath("/");
    final String BASE_PATH=SITE_BASE+"/";
    final String RES_PATH=SITE_BASE+"/WEB-INF/res";
    ByteArrayOutputStream baos = new ByteArrayOutputStream(100000000);
    Page pMaster = new Page(baos, RES_PATH+"/masterpage/masterpage.res");
    String sOut="<br><font color=blue>Blob Most Accessed Page<details></font><br><br>We now have following blobs....<br>";
    DB db=new DB();
    db.query("select distinct blob_id from blobs order by blob_id"); // we should obtain the blob information from another blob table instead of io table
    int count=0;
    sOut+="<form name='form1' action='most_accessed_page.jsp' method=post>";
    sOut+="<br><br>Select Blob:<select name=blob_id>";
    while(db.moveNext())
    {
        sOut+="<option value='"+db.geti(1)+"'>blob "+db.geti(1);
    }
    sOut+="</select>";
    sOut+="&nbsp &nbsp &nbsp version no.:<input type=text size=2 value='1' name='version'><br>";
    sOut+="<br><br><INPUT TYPE=RADIO NAME='access_type' VALUE='0' checked>Read";
    sOut+="<INPUT TYPE=RADIO NAME='access_type' VALUE='1'>Write";
    sOut+="<INPUT TYPE=RADIO NAME='access_type' VALUE='2'>Read & Write<br>";
    sOut+="Number of pages to display: <input type=text name=number_to_display value=100 size=5 />";
    // sOut+="<br><br>Number to display:<select name=number_to_display>";
    //sOut+="<option value='5'>top 5";
    //sOut+="<option value='10'>top 10";
    //sOut+="<option value='20'>top 20";
    //sOut+="<option value='50'>top 50</select>";
    sOut+="<br><br><br><br><br><input type='submit' value='submit'/> &nbsp &nbsp &nbsp";
    sOut+="<input type='reset' value='clear all' onClick='return confirm(\"Are you sure you want to reset the form?\")'/>";
    sOut+="</form>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
