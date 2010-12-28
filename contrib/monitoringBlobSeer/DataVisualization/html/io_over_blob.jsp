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
<jsp:useBean id="io_over_blob" scope="session" class="mon.charts.bean.io_over_blob"/>
<%

    ServletContext sc = getServletContext();
    final String SITE_BASE = sc.getRealPath("/");
    final String BASE_PATH=SITE_BASE+"/";
    final String RES_PATH=SITE_BASE+"/WEB-INF/res";
    ByteArrayOutputStream baos = new ByteArrayOutputStream(100000000);
    Page pMaster = new Page(baos, RES_PATH+"/masterpage/masterpage.res");
    String sOut="<br>I/O distrobution over Blobs<br><br>";
    String metric=request.getParameter("metric");
    Double unit=Double.parseDouble(request.getParameter("unit"));
    int op=Integer.parseInt(request.getParameter("operation")); 
    //sOut+=" "+Double.parseDouble(request.getParameter("unit"))+"  "+request.getParameter("operation")+"  "+request.getParameter("metric");
    io_over_blob.setunit(unit);
    io_over_blob.setoperation(op);
    io_over_blob.setmetric(metric);
    String chartViewer=io_over_blob.getChartViewer(request,response);
    sOut+="<img src="+chartViewer+" border=0 usemap='#imageMap'>";
    String operation=null;
    if(op==0)
	operation="read";
    else if(op==1)
	operation="Write";
    else if(op==2)
	operation="Read & Write";
    String unit_str=null;
    if(unit==1)
	unit_str="Byte";
    else if(unit==1024)
	unit_str="KB";
    else if(unit==1048576)
	unit_str="MB";

    sOut+="<table border=1><tr><td>Metric: </td><td>"+metric+"</td></tr>";
    sOut+="<tr><td>Operation: </td><td>"+operation+"</td></tr>";
    sOut+="<tr><td>Unit: </td><td>"+unit_str+"</td></tr>";
    sOut+="</table>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
