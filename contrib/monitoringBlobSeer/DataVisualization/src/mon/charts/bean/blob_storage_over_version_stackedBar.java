/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author tylor
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
import mon.util.blob.*;
import mon.util.db.DatabaseFastDB;

import java.util.*;
import javax.servlet.http.HttpSession;
import org.jfree.chart.entity.StandardEntityCollection;
import org.jfree.data.general.DatasetUtilities;
import javax.servlet.http.*;
import org.jfree.data.category.DefaultCategoryDataset;

class version_size
{
    public int version;
    public double size;
    public version_size(int ver,double sz)
    {
        version=ver;
        size=sz;
    }
}
public class blob_storage_over_version_stackedBar
{
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
    public String getperspective()
    {
        return perspective;
    }
    public void setperspective(String pers)
    {
        perspective=pers;
    }
    private  CategoryDataset getDataSet()
    {
        DefaultCategoryDataset dataset =  new DefaultCategoryDataset();
        HashMap map=new HashMap<Integer,ArrayList>();
        DatabaseFastDB db=new DatabaseFastDB();
        DatabaseFastDB db2=new DatabaseFastDB();
        DatabaseFastDB db3=new DatabaseFastDB();        
        db.executeQuery("select distinct blob_id from blobs order by blob_id asc");
        while(db.next())
        {
            int blob_id=db.getInt(1);
            String sqlcmd = "";
            int version = db.getInt(1);
            if(perspective.equals("diff"))
                sqlcmd="select version, write_size from client_writes_"+blob_id+
                " order by version asc";
            else if(perspective.equals("visible"))
                sqlcmd="select version, max(blob_offset+write_size) from client_writes_"+
                blob_id+" group by version order by version asc";
            else if(perspective.equals("total"))
            	sqlcmd="select t2.version , sum(t1.write_size) from client_writes_"+blob_id +" t1 ,"
            	+"client_writes_"+blob_id+" t2 " +" where t1.version<=t2.version group by t2.version "+
             	" order by t2.version asc";
            db2.executeQuery(sqlcmd);
            while(db2.next())
            {
            	dataset.addValue(db2.getDouble(2),"V"+db2.getDouble(1),"Blob"+blob_id);
            }
            
        }
        return dataset;
    }
     public String getChartViewer(HttpServletRequest request, HttpServletResponse response)
     {
       JFreeChart chart = ChartFactory.createStackedBarChart3D(
                "", "Blob", "",
                getDataSet(), PlotOrientation.HORIZONTAL, true, true, false);
        chart.setBackgroundPaint(new Color(249, 231, 236));
        CategoryPlot plot = chart.getCategoryPlot();
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
