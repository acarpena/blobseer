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
import mon.util.blob.*;
import mon.util.db.DatabaseFastDB;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.entity.StandardEntityCollection;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.xy.*;

/**
 *
 * @author tylor
 */
public class io_over_page {

    public String blob_id;
    public String getblob_id()
    {
        return blob_id;
    }
    public void setblob_id(String id)
    {
        blob_id=id;
    }
    private XYDataset getDataSet()
    {
        XYSeries series = new XYSeries("");
        DatabaseFastDB db=new DatabaseFastDB();
        //db.query("select count(distinct blob_offset) from blob_pages where blob_id="+blob_id);
        //db.moveNext();
        //int count_of_page=db.geti(1);
        //int page_id[]=new int[count_of_page];
        db.executeQuery("select distinct page_index,count(*) from write_"+blob_id+
        		" group by page_index order by page_index asc");
        int i=1;
        while(db.next())
        {
            series.add(i++,db.getDouble(2));
        }

        XYDataset xyDataset = new XYSeriesCollection(series);
        return xyDataset;
    }
     public String getChartViewer(HttpServletRequest request, HttpServletResponse response) {
        JFreeChart chart = ChartFactory.createXYAreaChart
          ("", "Page ID", "Amount of I/Os",
        getDataSet(), PlotOrientation.VERTICAL, true, true, false);
        chart.setBackgroundPaint(new Color(249, 231, 236));
        ChartRenderingInfo info = null;
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
        catch (Exception e) {
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
