<%@ page import="lia.Monitor.Store.Fast.DB" %>
<%@ page import="java.io.*" %>
<%@ page import="lia.web.utils.Page" %>
<%
    ServletContext sc = getServletContext();
    final String SITE_BASE = sc.getRealPath("/");
    final String BASE_PATH=SITE_BASE+"/";
    final String RES_PATH=SITE_BASE+"/WEB-INF/res";
    ByteArrayOutputStream baos = new ByteArrayOutputStream(100000000);
    Page pMaster = new Page(baos, RES_PATH+"/masterpage/masterpage.res");
    String sOut="<br><font color=blue>Blob storage over versions <details></font><br><br>We now have following blobs....<br>";
    DB db=new DB();
    db.query("select distinct blob_id from blobs order by blob_id asc");
    int count=0;
    sOut+="<form name='form1' action='blob_storage_over_version_bar.jsp' method=post>";
    sOut+="<br><br><select name=blob_id>";
    while(db.moveNext())
    {
        sOut+="<option value='"+db.geti(1)+"'>blob "+db.geti(1);
    }
    sOut+="</select>";
    sOut+="<br><br>Perspective: <select name=perspective><option value='diff'>diff<option value='total'>total<option value='visible'>visible</select>";
    sOut+="<INPUT TYPE=RADIO NAME='chart_type' VALUE='xyAreaChart'>XY Area Chart";
    sOut+="<INPUT TYPE=RADIO NAME='chart_type' VALUE='barChart'>Bar Chart<br>";
    sOut+="<br><br><br><br><br><br><input type='submit' value='submit'/> &nbsp &nbsp &nbsp";
    sOut+="<input type='reset' value='clear all' onClick='return confirm(\"Are you sure you want to reset the form?\")'/>";
    sOut+="</form>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
