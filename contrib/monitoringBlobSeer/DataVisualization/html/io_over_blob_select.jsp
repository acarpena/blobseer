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
    String sOut="<br>Blob I/O over blobs (3D Pie Chart)<br>";
    sOut+="<form name='form1' action='io_over_blob.jsp' method=post>";
    sOut+="<br>";
    sOut+="<br><br>Displaying Metric: <INPUT TYPE=RADIO NAME='metric' VALUE=num_of_access checked>number of accesses";
    sOut+="<INPUT TYPE=RADIO NAME=metric VALUE=total_size>total size";
    sOut+="<br><br>Operation:<select name=operation>Operation";
    sOut+="<option value='0'>Read";
    sOut+="<option value='1'>Write";
    sOut+="<option value='2'>Read & Write</select>";
    
    sOut+="<br><br>Unit:<select name=unit>Unit";
    sOut+="<option value='1048576'>MB";
    sOut+="<option value='1024'>KB";
    sOut+="<option value='1'>Byte</select>";
    
    sOut+="<br><br><br><br><br><input type='submit' value='submit'/> &nbsp &nbsp &nbsp";
    sOut+="<input type='reset' value='clear all' onClick='return confirm(\"Are you sure you want to reset the form?\")'/>";
    sOut+="</form>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
