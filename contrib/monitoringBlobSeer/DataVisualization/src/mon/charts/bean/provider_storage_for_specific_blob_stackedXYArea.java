/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package mon.charts.bean;
import java.awt.Color;
import java.awt.image.*;
import java.io.PrintWriter;
import java.util.ArrayList;
import org.jfree.chart.*;
import javax.servlet.http.*;
import lia.Monitor.Store.Fast.DB;
import mon.util.db.DatabaseFastDB;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.entity.StandardEntityCollection;
import org.jfree.chart.plot.Plot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.xy.*;


/**
 *
 * @author tylor
 */
public class provider_storage_for_specific_blob_stackedXYArea
{
    public String blob_id;
    private double unit;
    public void setunit(double un)
    {
    	unit=un;
    }
    public double getunit()
    {
        return unit;
    }
    public String getblob_id()
    {
        return blob_id;
    }
    public void setblob_id(String id)
    {
        blob_id=id;
    }
    private CategoryTableXYDataset getDataSet()
    {
        CategoryTableXYDataset dataset = new CategoryTableXYDataset();
        DatabaseFastDB db=new DatabaseFastDB();
        DatabaseFastDB db2=new DatabaseFastDB();
        String sqlcmd = "select distinct version,watermark from client_writes_"+blob_id+" order by version asc";
        db.executeQuery(sqlcmd);
        while(db.next())
        {
            int version=db.getInt(1);
            String watermark=db.getString(2);
            sqlcmd="select node, count(page_index)*(select page_size from blobs where " +
            		"blob_id='"+blob_id+"') from write_" +blob_id+ " where watermark='"+watermark+
            		"'  group by node order by node asc";
            db2.executeQuery(sqlcmd);
            int i=0;
            while(db2.next())
            {
                dataset.add(i,db2.getDouble(2)/unit,""+version);
                i++;
            }
        }
        return dataset;
        
    }
     public String getChartViewer(HttpServletRequest request, HttpServletResponse response) 
     {
       String unit_str="Size";
       if(unit==1024)
           unit_str="Size (KB)";
       else if(unit==1048576)
           unit_str="Size (MB)";
       else  // (unit==1)
           unit_str="Size (Byte)";
        final JFreeChart chart = ChartFactory.createStackedXYAreaChart(
            "",      // chart title
            "provider",                // domain axis label
            unit_str,                   // range axis label
            getDataSet(),                   // data
            PlotOrientation.VERTICAL,  // orientation
            true,                      // include legend
            true,
            false
        );
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
           // handel your exception here
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
