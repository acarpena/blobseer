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
    db.query("select distinct blob_id from blobs order by blob_id"); // we should obtain the blob information from another blob table instead of io table
    int count=0;
    sOut+="<form name='form1' action='provider_storage_for_specific_blob.jsp' method=post>";
    sOut+="<select name=blob_id>";
    while(db.moveNext())
    {
        sOut+="<option value='"+db.geti(1)+"'>blob "+db.geti(1);
    }
    sOut+="</select>";
    sOut+="<br><br>Showing Unit:<select name=unit><option value='1048576'>MB<option value='1024'>KB<option value='1'>Byte</select>";
    sOut+="<br><br><br><br><br><br><input type='submit' value='submit'/> &nbsp &nbsp &nbsp";
    
    sOut+="<input type='reset' value='clear all' onClick='return confirm(\"Are you sure you want to reset the form?\")'/>";
    sOut+="</form>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>