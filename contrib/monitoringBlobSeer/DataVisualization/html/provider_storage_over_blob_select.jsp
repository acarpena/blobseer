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
    String sOut="<br>Provider storage over blob<br>";
    sOut+="<form name='form1' action='provider_storage_over_blob.jsp' method=post>";
    sOut+="<br><br>Chart Type: <select name=chart_type><option value='xyAreaChart'>Stacked XY Area Chart<option value='barChart'>Stacked Bar Chart</select>";
    sOut+="<br><br><br><br><br><br><input type='submit' value='submit'/> &nbsp &nbsp &nbsp";
    
    sOut+="<input type='reset' value='clear all' onClick='return confirm(\"Are you sure you want to reset the form?\")'/>";
    sOut+="</form>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
