/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package mon.charts.bean;
import java.awt.Color;
import java.awt.image.*;
import java.io.PrintWriter;
import org.jfree.chart.*;
import javax.servlet.http.*;
import lia.Monitor.Store.Fast.DB;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.entity.StandardEntityCollection;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.xy.*;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.general.Dataset;
/**
 *
 * @author tylor
 */

/*
 * 
 * this chart is obsolete, since we can not get the version info from read.
 *
 */
public class no_of_access_per_version
{ 
    public int blob_id;
    public int operation;
    public String op_str;
    public String chart_type; 
    private DefaultCategoryDataset barDataset = null;
    private XYDataset xyDataset = null;

    public String getchart_type()
    {
        return chart_type;
    }
    public void setchart_type(String type)
    {
        chart_type=type;
    }
    public int getblob_id()
    {
        return blob_id;
    }
    public void setblob_id(int id)
    {
        blob_id=id;
    }
    public int getoperation()
    {
        return operation;
    }
    public void setoperation(int op)
    {
        operation=op;
        if(operation==0)
        	op_str="Read";
        else if(operation==1)
        	op_str="Write";
        else if(operation==2)
        	op_str="Read & Write";
    }

    private void getDataSet()
    {
        XYSeries series = null;
        if(chart_type.equals("barChart"))
        {
          barDataset = new DefaultCategoryDataset();
        }
        else if(chart_type.equals("xyAreaChart"))
        {
           series = new XYSeries("");
        }
        DB db=new DB();
        String sqlcmd=null;
        if(operation==2)
        {
            sqlcmd="select vman_writes.version, " +
           "count(provider_writes.watermark) " + 
           //"+count(provider_reads.timestamp) " +
           " as num " +
           " from client_writes_" + blob_id + " vman_writes, " +
           " write_" + blob_id + " provider_writes " +
           //", read_" + blob_id + " provider_reads " +
           " where provider_writes.watermark=vman_writes.watermark " +
           " group by vman_writes.version order by vman_writes.version asc";
           db.query(sqlcmd);
        }
        else
        {
            sqlcmd="select vman_writes.version, count(provider_writes.watermark) as num " +
            " from client_writes_" + blob_id + " vman_writes, " +
            " write_" + blob_id + " provider_writes " +
            " where provider_writes.watermark=vman_writes.watermark "+
            " group by vman_writes.version order by vman_writes.version asc";

            db.query(sqlcmd);   
        }
        if(chart_type.equals("barChart"))
        {
            while(db.moveNext())
            {
                barDataset.addValue(db.getd(2),op_str,"v"+db.getd(1));
            }
           
        }
        else if(chart_type.equals("xyAreaChart"))
        {
            while(db.moveNext())
            {
                series.add(db.getd(1),db.getd(2));
            }
            
            xyDataset = new XYSeriesCollection(series);
        }
    }

    public String getChartViewer(HttpServletRequest request, HttpServletResponse response)
    {
        getDataSet();

        JFreeChart chart = null;
        if(chart_type.equals("xyAreaChart"))
        {
           chart = ChartFactory.createXYAreaChart("", "version", "No. of accesses",
                   xyDataset, PlotOrientation.VERTICAL, true, true, false);
        }
        else if(chart_type.equals("barChart"))
        {
          chart=ChartFactory.createBarChart3D("", "version", "No. of accesses",
                  barDataset, PlotOrientation.VERTICAL, true, true, false);
        }
        chart.setBackgroundPaint(new Color(249, 231, 236));
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
