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
<jsp:useBean id="blob_storage_over_version_bar" class="mon.charts.bean.blob_storage_over_version_bar" scope="session"/>
<jsp:setProperty name="blob_storage_over_version_bar" property="blob_id"/>
<jsp:setProperty name="blob_storage_over_version_bar" property="perspective"/>
<%
    ServletContext sc = getServletContext();
    final String SITE_BASE = sc.getRealPath("/");
    final String BASE_PATH=SITE_BASE+"/";
    final String RES_PATH=SITE_BASE+"/WEB-INF/res";
    ByteArrayOutputStream baos = new ByteArrayOutputStream(100000000);
    Page pMaster = new Page(baos, RES_PATH+"/masterpage/masterpage.res");
    String sOut="<br><font color=blue>Blob storage over versions</font><br><br>";
    String blob_id=request.getParameter("blob_id");
    String perspective=request.getParameter("perspective");
    blob_storage_over_version_bar.setblob_id(blob_id);
    blob_storage_over_version_bar.setperspective(perspective);
    blob_storage_over_version_bar.setchart_type(request.getParameter("chart_type"));
    String chartViewer=blob_storage_over_version_bar.getChartViewer(request,response);
    sOut+="<img src="+chartViewer+" border=0 usemap='#imageMap'>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
