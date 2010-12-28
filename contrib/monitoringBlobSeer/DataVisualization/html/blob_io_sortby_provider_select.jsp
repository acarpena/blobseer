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
    String sOut="<br><font color=blue>Blob I/O over provider details, please select...</font><br><br>We now have following blobs....<br>";
    DB db=new DB();
    db.query("select distinct blob_id from blobs order by blob_id");

    int count=0;
    sOut+="<form name='form1' action='blob_io_sortby_provider.jsp' method=post>";
    sOut+="<table align=center border=0>";
    sOut+="<tr>";
    while(db.moveNext())
    {
        sOut+="<td><input type='RADIO' name='blob' value='"+db.geti(1)+"'/><font color=green font-weight=bold><b> blob "+db.geti(1)+"</b></font></td>";
        if((++count)%5==0)
            sOut+="</tr><tr>";
    }
    sOut+="</tr></table>";//<input type='checkbox' name='blob' value='all' /><b>select all blobs</b></font>";
    sOut+="<br><br>Sort the <select name=number_to_display><option value=5>top 5<option value=10>top 10<option value=20>top 20</select> providers by <select name=sort><option value=read>Read Amount<option value=write>Write Amount</select> descendantly<br><br>";
    sOut+="<br><input type='submit' value='submit'/> &nbsp &nbsp &nbsp";
    sOut+="<input type='reset' value='clear all' onClick='return confirm(\"Are you sure you want to reset the form?\")'/>";
    sOut+="</form>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
