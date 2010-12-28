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
<jsp:useBean id="most_accessed_page" scope="session" class="mon.charts.bean.most_accessed_page" scope="session"/>
<jsp:setProperty name="most_accessed_page" property="blob_id"/>
<jsp:setProperty name="most_accessed_page" property="version"/>
<jsp:setProperty name="most_accessed_page" property="access_type"/>
<%
    ServletContext sc = getServletContext();
    final String SITE_BASE = sc.getRealPath("/");
    final String BASE_PATH=SITE_BASE+"/";
    final String RES_PATH=SITE_BASE+"/WEB-INF/res";
    ByteArrayOutputStream baos = new ByteArrayOutputStream(100000000);
    Page pMaster = new Page(baos, RES_PATH+"/masterpage/masterpage.res");
    String sOut="<br><font color=blue>Blob Most Accessed Pages</font><br><br>";
    String blob_id=request.getParameter("blob_id");
    String version=request.getParameter("version");
    String access_type=request.getParameter("access_type");
    String number_to_display=request.getParameter("number_to_display");
    //sOut+="-->"+number_to_display;
    most_accessed_page.setblob_id(Integer.parseInt(blob_id));
    most_accessed_page.setversion(Integer.parseInt(version));
    most_accessed_page.setaccess_type(Integer.parseInt(access_type));
    most_accessed_page.setnumber_to_display(Integer.parseInt(number_to_display));
    String chartViewer=most_accessed_page.getChartViewer(request,response);
    sOut+="<img src="+chartViewer+" border=0 usemap='#imageMap'>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
