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
<jsp:useBean id="blob_version_over_page" scope="session" class="mon.charts.bean.blob_version_over_page" scope="session"/>
<jsp:setProperty name="blob_version_over_page" property="blob_id"/>
<jsp:setProperty name="blob_version_over_page" property="version"/>
<jsp:setProperty name="blob_version_over_page" property="chart_type"/>

<%
    ServletContext sc = getServletContext();
    final String SITE_BASE = sc.getRealPath("/");
    final String BASE_PATH=SITE_BASE+"/";
    final String RES_PATH=SITE_BASE+"/WEB-INF/res";
    ByteArrayOutputStream baos = new ByteArrayOutputStream(100000000);
    Page pMaster = new Page(baos, RES_PATH+"/masterpage/masterpage.res");
    String sOut="<br><font color=blue>Blob version over pages</font><br><br>";
    String blob_id=request.getParameter("blob_id");
    String version=request.getParameter("version");
    String chart_type=request.getParameter("chart_type");
    String chartViewer=null;
    
	blob_version_over_page.setblob_id(blob_id);
	blob_version_over_page.setversion(version);
	blob_version_over_page.setchart_type(chart_type);
	chartViewer=blob_version_over_page.getChartViewer(request,response);
 
    sOut+="<img src="+chartViewer+" border=0 usemap='#imageMap'>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
