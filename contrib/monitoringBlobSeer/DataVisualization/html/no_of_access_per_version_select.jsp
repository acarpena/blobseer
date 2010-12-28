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
    String sOut="<br><font color=blue>no. of access per version<details></font><br><br>We now have following blobs....<br>";
    DB db=new DB();
    db.query("select distinct blob_id from blobs order by blob_id"); 

    int count=0;
    sOut+="<form name='form1' action='no_of_access_per_version.jsp' method=post>";
    sOut+="<br><br>Select Blob:<select name=blob_id>";
    while(db.moveNext())
    {
        sOut+="<option value='"+db.geti(1)+"'>blob "+db.geti(1);
    }
    sOut+="</select>";
    sOut+="<br><br>Chart Type: <input type=radio name='chart_type' value='barChart' checked>3D Bar Chart";
    sOut+="&nbsp &nbsp &nbsp<input type=radio name='chart_type' value='xyAreaChart'>XY Area Chart";
    sOut+="<br><br>Type of operation: <INPUT TYPE=RADIO NAME='access_type' VALUE='0' checked>Read";
    sOut+="<INPUT TYPE=RADIO NAME='access_type' VALUE='1'>Write";
    sOut+="<INPUT TYPE=RADIO NAME='access_type' VALUE='2'>Read & Write<br>";
    //sOut+="<br><br>Number to display:<select name=number_to_display>";
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
