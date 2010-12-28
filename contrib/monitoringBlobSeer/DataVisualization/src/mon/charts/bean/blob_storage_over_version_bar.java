/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package mon.charts.bean;
import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.PrintWriter;
import org.jfree.chart.*;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.category.CategoryDataset;
import lia.Monitor.Store.Fast.DB;
import mon.util.db.DatabaseFastDB;

import javax.servlet.http.HttpSession;
import org.jfree.chart.entity.StandardEntityCollection;
import javax.servlet.http.*;
import org.jfree.data.category.*;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;


/**
 *
 * @author tylor
 */
public class blob_storage_over_version_bar
{
    public String blob_id;
    public String perspective;
    public String chart_type;
    public String getchart_type()
    {
        return chart_type;
    }
    public void setchart_type(String type)
    {
        chart_type=type;
    }
    public String getblob_id()
    {
        return blob_id;
    }
    public void setblob_id(String id)
    {
        blob_id=id;
    }
    public String getperspective()
    {
        return perspective;
    }
    public void setperspective(String pers)
    {
        perspective=pers;
    }
    private CategoryDataset getDataSet()
    {
        DefaultCategoryDataset dataset =  new DefaultCategoryDataset();
        DatabaseFastDB db=new DatabaseFastDB();
        DatabaseFastDB db2=new DatabaseFastDB();
        //ArrayList version_size_list=new ArrayList();
        db.executeQuery("select distinct version from client_writes_"+blob_id);
        while(db.next())
        {
            String sqlcmd = "";
            int version = db.getInt(1);
            if(perspective.equals("diff"))
                sqlcmd="select write_size from client_writes_"+blob_id+" where version="+version;
            else if(perspective.equals("total"))
            	sqlcmd="select sum(write_size) from client_writes_"+blob_id+" where version<="+version;
            else if(perspective.equals("visible"))
                sqlcmd="select max(blob_offset+write_size) from client_writes_"+blob_id+" where version<="+version;
            //System.out.println(sqlcmd);
            db2.executeQuery(sqlcmd);
            db2.next();
            dataset.addValue(db2.getDouble(1),"size",""+version);
           //version_size_list.add(new version_size(version,db2.geti(1)));
        }
     
        return dataset;
    }
    private XYDataset getDataSet_1()
    {
        XYSeries series = new XYSeries("");
        DatabaseFastDB db=new DatabaseFastDB();
        DatabaseFastDB db2=new DatabaseFastDB();
        //ArrayList version_size_list=new ArrayList();
        db.executeQuery("select distinct version from client_writes_"+blob_id);
        while(db.next())
        {
            String sqlcmd = "";
            int version = db.getInt(1);
            if(perspective.equals("diff"))
                sqlcmd="select write_size from client_writes_"+blob_id+" where version="+version;
            else if(perspective.equals("total"))
            	sqlcmd="select sum(write_size) from client_writes_"+blob_id+" where version<="+version;
            else if(perspective.equals("visible"))
                sqlcmd="select max(blob_offset+write_size) from client_writes_"+blob_id+" where version<="+version;
            //System.out.println(sqlcmd);
            db2.executeQuery(sqlcmd);
            db2.next();
            series.add(version,db2.getDouble(1));
        }
        XYDataset xyDataset = new XYSeriesCollection(series);
        return xyDataset;
    }
     public String getChartViewer(HttpServletRequest request, HttpServletResponse response)
     {
       JFreeChart chart=null;
       //System.out.println(chart_type);
       if((chart_type!=null) && (chart_type.equals("barChart")))
       {
           chart = ChartFactory.createBarChart(
                "", "version", "",
                getDataSet(), PlotOrientation.VERTICAL, true, true, false);
       }
       else  // (chart_type.equals("xyAreaChart"))
       {
           chart = ChartFactory.createXYAreaChart(
                "", "version", "",
                getDataSet_1(), PlotOrientation.VERTICAL, true, true, false);
       }
       chart.setBackgroundPaint(new Color(249, 231, 236));
       // plot.getRenderer().setSeriesPaint(0, new Color(30, 100, 175));
       // plot.getRenderer().setSeriesPaint(1, new Color(90, 190, 110));
       // plot.getRenderer().setSeriesPaint(2, new Color(225, 45, 45));
        ChartRenderingInfo info = null;
        HttpSession session = request.getSession();
        try
        {
          //Create RenderingInfo object
          response.setContentType("text/html");
          info = new ChartRenderingInfo(new StandardEntityCollection());
          BufferedImage chartImage = chart.createBufferedImage(640, 400, info);
          // putting chart as BufferedImage in session,
          // thus making it available for the image reading action Action.
          session.setAttribute("chartImage", chartImage);
          PrintWriter writer = new PrintWriter(response.getWriter());
          ChartUtilities.writeImageMap(writer, "imageMap", info,true);
          writer.flush();
        }
        catch (Exception e)
        {
           e.printStackTrace();
        }
        String pathInfo = "http://";
        pathInfo += request.getServerName();
        int port = request.getServerPort();
        pathInfo += ":"+String.valueOf(port);
        pathInfo += request.getContextPath();
        String chartViewer = pathInfo + "/servlet/ChartViewer";
        return chartViewer;
     }
}
