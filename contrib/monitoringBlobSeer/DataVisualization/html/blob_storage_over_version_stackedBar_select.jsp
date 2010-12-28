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
    String sOut="<br><font color=blue>Blob storage over versions <Stacked Bar></font><br><br>";
    sOut+="<form name='form1' action='blob_storage_over_version_stackedBar.jsp' method=post>";
    sOut+="<br><br>Perspective: <select name=perspective><option value='diff'>diff<option value='total'>total<option value='visible'>visible</select>";
   // sOut+="<br><br><br>Chart Type: <br><br><INPUT TYPE=RADIO NAME='chart_type' VALUE='barChart'>Stacked Bar Chart<br>";
   // sOut+="<INPUT TYPE=RADIO NAME='chart_type' VALUE='xyAreaChart'>Stacked XY Area Chart";
   // sOut+="(Number of Versions to display: <input type=text name=num_to_display value=2 size=2/>)<br><br>";
    sOut+="<br><br><br><br><br><input type='submit' value='submit'/> &nbsp &nbsp &nbsp";
    sOut+="<input type='reset' value='clear all' onClick='return confirm(\"Are you sure you want to reset the form?\")'/>";
    sOut+="</form>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
