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
    String sOut="<br><font color=blue>Blob storage over versions Stacked XY Area</font><br><br>";
    sOut+="<form name='form1' action='blob_storage_over_version_stackedXYArea.jsp' method=post>";
    sOut+="Number of version to display: <input type=text name=num_to_display value=2 size=2><br>";
    sOut+="<br>Showing Unit:<select name=unit><option value='1048576'>MB<option value='1024'>KB<option value='1'>Byte</select>";
    sOut+="<br><br><br><br><br><br><input type='submit' value='submit'/> &nbsp &nbsp &nbsp";
    sOut+="<input type='reset' value='clear all' onClick='return confirm(\"Are you sure you want to reset the form?\")'/>";
    sOut+="</form>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
