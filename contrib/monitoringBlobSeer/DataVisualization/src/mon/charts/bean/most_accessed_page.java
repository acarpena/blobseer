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
import java.util.*;
import javax.servlet.http.HttpSession;
import org.jfree.chart.entity.StandardEntityCollection;
import org.jfree.data.general.DatasetUtilities;
import org.jfree.data.xy.CategoryTableXYDataset;

import javax.servlet.http.*;
import org.jfree.data.category.*;
/**
 *
 * @author tylor
 */
public class most_accessed_page
{
    public int number_to_display;
    public int access_type;
    //0:read, 1:write, 2 read & write
    public int blob_id;
    public int version;
    public int getnumber_to_display()
    {
        return number_to_display;
    }
    public void setnumber_to_display(int num)
    {
        number_to_display = num;
    }
    public int getaccess_type()
    {
        return access_type;
    }
    public void setaccess_type(int type)
    {
        access_type = type;
    }
    public int getversion()
    {
        return version;
    }
    public void setversion(int ver)
    {
        version = ver;
    }
    public int getblob_id()
    {
        return blob_id;
    }
    public void setblob_id(int id)
    {
        blob_id = id;
    }
    private  CategoryTableXYDataset getDataSet()
    {
    	CategoryTableXYDataset dataset =  new CategoryTableXYDataset();
        String sqlcmd  = null;
        if(access_type == 0)
            sqlcmd =
                "select (blob_pages.blob_offset)/(blob_pages.page_size) as page_id,count(id) as num"+
                " from blob_io, blob_pages where blob_pages.blob_id="+blob_id+
                " and blob_pages.written_version="+version+" and operation=0" +
                " and blob_io.blob_id=blob_pages.blob_id"+
                " and blob_io.watermark=blob_pages.watermark"+
                " group by page_id order by num desc,page_id asc";
        else if(access_type == 1)
            sqlcmd =
                "select (blob_pages.blob_offset)/(blob_pages.page_size) as page_id,count(id) as num"+
                " from blob_io, blob_pages where blob_pages.blob_id="+blob_id+
                " and blob_pages.written_version="+version+" and operation=1"+
                " and blob_io.blob_id=blob_pages.blob_id"+
                " and blob_io.watermark=blob_pages.watermark"+
                " group by page_id order by num desc,page_id asc";
        else if(access_type == 2)
            sqlcmd =
                "select (blob_pages.blob_offset)/(blob_pages.page_size) as page_id,count(id) as num"+
                " from blob_io, blob_pages where blob_pages.blob_id="+blob_id+
                " and blob_pages.written_version="+version+
                " and blob_io.blob_id=blob_pages.blob_id" +
                " and blob_io.watermark=blob_pages.watermark"+
                " group by page_id order by num desc,page_id asc";
        else
            return null;
        //System.out.println(sqlcmd);
        DB db = new DB();
        db.query(sqlcmd);
        int i=1;
        while(i++ <= number_to_display && db.moveNext())
        {
            double page_id=db.getd(1)+1;
            dataset.add(page_id,db.getd(2),"Blob ["+blob_id+"]  Version ["+version+"]");
        }
        return dataset;
    }
    public String getChartViewer(HttpServletRequest request, HttpServletResponse response)
     {
       JFreeChart chart = ChartFactory.createStackedXYAreaChart(
                "", "page ID", "Access amount",
                getDataSet(), PlotOrientation.VERTICAL, true, true, false);
        chart.setBackgroundPaint(new Color(249, 231, 236));
        //CategoryPlot plot = chart.getCategoryPlot();
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
          BufferedImage chartImage = chart.createBufferedImage(600,480, info);
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
