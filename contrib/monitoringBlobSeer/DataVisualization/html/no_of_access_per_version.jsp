<%@ page  import="java.awt.*" %>
<%@ page  import="java.io.*" %>
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
<%@ page  import="org.jfree.chart.*" %>
<%@ page  import="org.jfree.chart.plot.PlotOrientation" %>
<%@ page  import="org.jfree.data.category.DefaultCategoryDataset" %>
<%@ page  import="org.jfree.data.general.DefaultPieDataset" %>
<%@ page  import="org.jfree.data.xy.*" %>
<%@ page  import="org.jfree.data.* "%>
<jsp:useBean id="no_of_access_per_version" class="mon.charts.bean.no_of_access_per_version" scope="session">
<jsp:setProperty name="no_of_access_per_version" property="blob_id"/>
<jsp:setProperty name="no_of_access_per_version" property="operation"/>
<jsp:setProperty name="no_of_access_per_version" property="chart_type"/>
</jsp:useBean>
<%
    String blob_id=request.getParameter("blob_id");
    blob_id=String.valueOf(blob_id.charAt(blob_id.length()-1));
    String chart_type=request.getParameter("chart_type");
    String access_type=request.getParameter("access_type");
    
    ServletContext sc = getServletContext();
    final String SITE_BASE = sc.getRealPath("/");
    final String BASE_PATH=SITE_BASE+"/";
    final String RES_PATH=SITE_BASE+"/WEB-INF/res";
    ByteArrayOutputStream baos = new ByteArrayOutputStream(100000);
    Page pMaster = new Page(baos, RES_PATH+"/masterpage/masterpage.res");
    String sOut="<br>No. of Access per version<br><br>";
    no_of_access_per_version.setblob_id(Integer.parseInt(blob_id));
    no_of_access_per_version.setoperation(Integer.parseInt(access_type));
    no_of_access_per_version.setchart_type(chart_type);
    String chartViewer=no_of_access_per_version.getChartViewer(request,response);
    //sOut+=chartViewer;
    sOut+="<img src="+chartViewer+" border=0 usemap='#imageMap'>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
    
%>
