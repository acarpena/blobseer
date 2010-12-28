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
<jsp:useBean id="io_over_page" class="mon.charts.bean.io_over_page" scope="session">
<jsp:setProperty name="io_over_page" property="blob_id"/>
</jsp:useBean>
<%
    String blob_id=request.getParameter("blob_id");
    blob_id=String.valueOf(blob_id.charAt(blob_id.length()-1));
    String server =request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+"/";
    lia.Monitor.Store.Fast.TempMemWriterInterface tmw = null;
    String sError = "";
    try
    {
        lia.Monitor.Store.TransparentStoreFast store = (lia.Monitor.Store.TransparentStoreFast) lia.Monitor.Store.TransparentStoreFactory.getStore();
        tmw = store.getTempMemWriter();
    }
    catch (Throwable t)
    {
        sError = t.getMessage();
    }
    ServletContext sc = getServletContext();
    final String SITE_BASE = sc.getRealPath("/");
    final String BASE_PATH=SITE_BASE+"/";
    final String RES_PATH=SITE_BASE+"/WEB-INF/res";
    ByteArrayOutputStream baos = new ByteArrayOutputStream(100000);
    Page pMaster = new Page(baos, RES_PATH+"/masterpage/masterpage.res");
    String sOut="<br>Blob <font color=red>< "+blob_id+" ></font> I/O distribution on pages<br><br>";
    io_over_page.setblob_id(blob_id);
    String chartViewer=io_over_page.getChartViewer(request,response);
    sOut+="<img src="+chartViewer+" border=0 usemap='#imageMap'>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
    
%>
