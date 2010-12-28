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
import mon.util.db.DatabaseFastDB;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.CategoryAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.entity.StandardEntityCollection;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.renderer.category.CategoryItemRenderer;
import org.jfree.data.category.CategoryDataset;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.xy.CategoryTableXYDataset;
import org.jfree.data.xy.TableXYDataset;
import org.jfree.ui.RectangleInsets;

/**
 *
 * @author tylor
 */
public class blob_storage_over_version_stackedXYArea
{
    private int num_to_display;
    private double unit;
    public String perspective;
    public String getperspective()
    {
        return perspective;
    }
    public void setperspective(String pers)
    {
        perspective=pers;
    }
    public void setnum_to_display(int num)
    {
        num_to_display=num;
    }
    public int getnum_to_display()
    {
        return num_to_display;
    }    
    public void setunit(double un)
    {
        unit=un;
    }
    public double getunit()
    {
        return unit;
    }
    public DefaultCategoryDataset getDataSet()
    {
        DefaultCategoryDataset dataset = new DefaultCategoryDataset();
        DatabaseFastDB db=new DatabaseFastDB();
        DatabaseFastDB db2=new DatabaseFastDB();
        String sqlcmd="select distinct blob_id from blobs order by blob_id asc";
        db.executeQuery(sqlcmd);
        while(db.next())
        {
            int blob_id=db.getInt(1);
            
            sqlcmd="select version, write_size from client_writes_"+blob_id+
            " order by version limit "+num_to_display;
            db2.executeQuery(sqlcmd);
            //System.out.println(sqlcmd);
            while(db2.next())
            {
                if(db2.getDouble(2)!=0)
                    dataset.addValue(db2.getDouble(2)/unit,"v"+db2.getInt(1),"blob"+blob_id);
            }
            sqlcmd="select sum(write_size) from client_writes_"+blob_id+
                    " where version >" + num_to_display;
            db2.executeQuery(sqlcmd);
            //System.out.println(sqlcmd);
            if(db2.next())
            {
                if(db2.getDouble(1)!=0)
                    dataset.addValue(db2.getDouble(1)/unit,"Other versions","blob"+blob_id);
                //System.out.println(db2.geti(1));
            }   
        }
        return dataset;
    }

     public String getChartViewer(HttpServletRequest request, HttpServletResponse response) {
       String unit_str="Size";
       if(unit==1024)
           unit_str="Size (KB)";
       else if(unit==1048576)
           unit_str="Size (MB)";
       else if(unit==1)
           unit_str="Size (Byte)";

       final JFreeChart chart = ChartFactory.createStackedAreaChart(
            "",      // chart title
            "Blob",                // domain axis label
            unit_str,                   // range axis label
            getDataSet(),                   // data
            PlotOrientation.VERTICAL,  // orientation
            true,                      // include legend
            true,
            false
        );

        chart.setBackgroundPaint(Color.white);

        final CategoryPlot plot = (CategoryPlot) chart.getPlot();
        plot.setForegroundAlpha(0.5f);
        plot.setBackgroundPaint(Color.white);
        plot.setDomainGridlinePaint(Color.white);
        plot.setRangeGridlinePaint(Color.white);

        final CategoryAxis domainAxis = plot.getDomainAxis();
        domainAxis.setLowerMargin(0.0);
        domainAxis.setUpperMargin(0.0);
        
        // change the auto tick unit selection to integer units only...
        final NumberAxis rangeAxis = (NumberAxis) plot.getRangeAxis();
        rangeAxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());
        final CategoryItemRenderer renderer = plot.getRenderer();
        renderer.setItemLabelsVisible(true);
        ///////////////////////////////////
        plot.setAxisOffset(new RectangleInsets(0, 1D, -1, 0));
        ChartRenderingInfo info;
        HttpSession session = request.getSession();
        try {
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
