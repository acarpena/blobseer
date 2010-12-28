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
public class memory_stackedXYArea {


     public CategoryTableXYDataset getDataSet()
     {
    	CategoryTableXYDataset dataset = new CategoryTableXYDataset();
        DB db = new DB();
        DB db2 = new DB();
        String sqlcmd="select split_part(mi_key,'/',3),mi_lastvalue from monitor_ids where " +
                "split_part(mi_key,'/',2)='ApMon_SysMon' and split_part(mi_key,'/',4)='mem_used'";
        db.query(sqlcmd);
        String host=null;
        double val=0,max_ram=8*1024;
        int i=0;
        while(db.moveNext())
        {
            host=db.gets(1);
            if(host.split("\\.").length==4) //check if ip format is correct
            {
                val=db.getd(2);
                if(val>max_ram)
                	val /=8;
                dataset.add(i++,val, "Used Memory");
            }
        }
        sqlcmd="select split_part(mi_key,'/',3),mi_lastvalue from monitor_ids where " +
                "split_part(mi_key,'/',2)='ApMon_SysMon' and split_part(mi_key,'/',4)='mem_free'";
        db.query(sqlcmd);
        i=0;
        while(db.moveNext())
        {
            host=db.gets(1);
            if(host.split("\\.").length==4) //check if ip format is correct
            {
                val=db.getd(2);
                if(val>max_ram)
                	val /=8;
                dataset.add(i++,val, "Free Memory");
            }
        }
        return dataset;

     }
       public String getChartViewer(HttpServletRequest request, HttpServletResponse response)
       {

          final JFreeChart chart = ChartFactory.createStackedXYAreaChart(
            "",      // chart title
            "Provider",                // domain axis label
            "Memory",                   // range axis label
            getDataSet(),                   // data
            PlotOrientation.VERTICAL,  // orientation
            true,                      // include legend
            true,
            false
        );

        chart.setBackgroundPaint(Color.white);
        /*
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
        */
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
