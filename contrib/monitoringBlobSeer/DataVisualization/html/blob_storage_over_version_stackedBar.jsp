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
<jsp:useBean id="blob_storage_over_version" scope="session" class="mon.charts.bean.blob_storage_over_version_stackedBar"/>
<%
    ServletContext sc = getServletContext();
    final String SITE_BASE = sc.getRealPath("/");
    final String BASE_PATH=SITE_BASE+"/";
    final String RES_PATH=SITE_BASE+"/WEB-INF/res";
    ByteArrayOutputStream baos = new ByteArrayOutputStream(100000000);
    Page pMaster = new Page(baos, RES_PATH+"/masterpage/masterpage.res");
    String sOut="<br><font color=blue>Blob storage over versions</font><br><br>";
    blob_storage_over_version.setperspective(request.getParameter("perspective"));
    //String chart_type=request.getParameter("chart_type");
    //if(chart_type.equals("barChart"))
    //{
        blob_storage_over_version.setperspective(request.getParameter("perspective"));
    	String chartViewer=blob_storage_over_version.getChartViewer(request,response);
    	sOut+="<img src="+chartViewer+" border=0 usemap='#imageMap'>";
    //}
    //else if(chart_type.equals("xyAreaChart"))
    //{
	
	//blob_storage_over_version_stackedXYArea.setnum_to_display(Integer.parseInt(request.getParameter("num_to_display")));
        //blob_storage_over_version_stackedXYArea.setunit(1);
	//String chartViewer=blob_storage_over_version_stackedXYArea.getChartViewer(request,response);
        //sOut+="<img src="+chartViewer+" border=0 usemap='#imageMap'>";
    //}
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
