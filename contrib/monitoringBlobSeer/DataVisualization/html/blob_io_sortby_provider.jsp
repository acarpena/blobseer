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
<jsp:useBean id="blob_io_sortby_provider" class="mon.charts.bean.blob_io_sortby_provider" scope="session"/>
<jsp:setProperty name="blob_io_sortby_provider" property="blob_id"/>
<jsp:setProperty name="blob_io_sortby_provider" property="sortBy"/>
<jsp:setProperty name="blob_io_sortby_provider" property="number_to_display"/>
<%
    ServletContext sc = getServletContext();
    final String SITE_BASE = sc.getRealPath("/");
    final String BASE_PATH=SITE_BASE+"/";
    final String RES_PATH=SITE_BASE+"/WEB-INF/res";
    ByteArrayOutputStream baos = new ByteArrayOutputStream(100000000);
    Page pMaster = new Page(baos, RES_PATH+"/masterpage/masterpage.res");
    //String blob_ids="";
    //String blob_arr[]=request.getParameterValues("blob");
    String blob_id=request.getParameter("blob");
    String sortBy=request.getParameter("sort");
    //String number_to_display=request.getParameter("number_to_display");
    String sOut="<br>I/Os of Blob <font color=red> ";
    /*for(int i=0;i<blob_arr.length;i++)
    {   
        if(blob_arr[i].equals("all"))
	{
	   sOut="<br>I/Os of <font color=red>All Blobs</font> over providers<br>";
	   blob_ids="all";
           break;
	}
	blob_ids+=blob_arr[i]+"#";
	sOut+=blob_arr[i]+", ";
    }*/
    sOut+="</font> detailed view </br>";
    blob_io_sortby_provider.setblob_id(blob_id);
    blob_io_sortby_provider.setsortBy(sortBy);
    //blob_io_sortby_provider.setnumber_to_display(Integer.parseInt(number_to_display));
    //sOut+="("+blob_ids+"  "+sortBy+")";
    String chartViewer=blob_io_sortby_provider.getChartViewer(request,response);
    sOut+="<img src="+chartViewer+" border=0 usemap='#imageMap'>";
    pMaster.append(sOut);
    pMaster.write();
    String s=new String(baos.toByteArray());
    out.println(s);
%>
