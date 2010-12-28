/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package mon.charts.bean;
import java.awt.Color;
import java.awt.image.*;
import java.io.PrintWriter;
import org.jfree.chart.*;
import org.jfree.chart.entity.*;
import javax.servlet.http.*;
import lia.Monitor.Store.Fast.DB;
import mon.util.blob.*;
import mon.util.db.DatabaseFastDB;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.chart.plot.*;

/**
 *
 * @author tylor
 */
public class blob_io_sortby_provider {
    public String blob_id;
    public String op_type;
    public String sqlcmd;
    public String sortBy;
    public int number_to_display;
   
    public int getnumber_to_display()
    {
        return number_to_display;
    }
    public void setnumber_to_display(int num)
    {
        number_to_display=num;
    }
    public String getblob_id()
    {
        return blob_id;
    }
    public void setblob_id(String id)
    {
        blob_id=id;
    }
    public String getsortBy()
    {
         return sortBy;
    }
    public void setsortBy(String sort)
    {
        sortBy=sort;
    }
    private DefaultCategoryDataset getDataSet()
    {
        DefaultCategoryDataset dataset = new DefaultCategoryDataset();
        DatabaseFastDB db=new DatabaseFastDB();
        DatabaseFastDB db2=new DatabaseFastDB();
        
        String sqlcmd=null;
        //String selected_blobs="(";
        /*
        if(blob_ids.equals("all"))
        {
            db.executeQuery("select distinct blob_id from blob_io");
            while(db.next())
                selected_blobs+=db.getInt(1)+",";
        }
        else
        {
            String blob_id_arr[]=blob_ids.split("#");
            for(int i=0;i<blob_id_arr.length;i++)
                selected_blobs+=blob_id_arr[i]+",";
        }
        selected_blobs=selected_blobs.substring(0,selected_blobs.length()-1)+")";
		*/
        /*
        if(sortBy.equals("rw"))
        {
            sqlcmd="select node,count(id) as num from blob_io where blob_id in "+selected_blobs+" group by node order by num desc limit "+number_to_display;
            db.query(sqlcmd);
            while(db.moveNext())
            {
                db2.query("select count(id) as num from blob_io where operation=0 and node='"+db.gets(1)+"' and blob_id in "+selected_blobs);
                dataset.addValue(db2.getd(1),"Read",db.gets(1));
                db2.query("select count(id) as num from blob_io where operation=1 and node='"+db.gets(1)+"' and blob_id in "+selected_blobs);
                dataset.addValue(db2.getd(1),"Write",db.gets(1));
            }
            
        }*/
        if(sortBy.equals("read"))
        {
            sqlcmd="select node , count(*) as num from  read_"+blob_id+
            " group by node order by num desc limit "+number_to_display;
            db.executeQuery(sqlcmd);
            while(db.next())
            {
            	String node=db.getString(1);
                db2.executeQuery("select count(*) from write_"+blob_id+" where node='"+node);
                dataset.addValue(db.getDouble(2),"Read",node);
                dataset.addValue(db2.getDouble(1),"Write",node);
            }
        }
        else if(sortBy.equals("write"))
        {
        	sqlcmd="select node , count(*) as num from  write_"+blob_id+
            " group by node order by num desc limit "+number_to_display;
            db.executeQuery(sqlcmd);
            while(db.next())
            {    	
               	String node=db.getString(1);
            	db2.executeQuery("select count(*) from read_"+blob_id+" where node='" + node + "'");
            	dataset.addValue(db.getDouble(2),"Write",node);
            	dataset.addValue(db2.getDouble(1),"Read",node);
                
            }
        }
        return dataset;
     }
     public String getChartViewer(HttpServletRequest request, HttpServletResponse response) 
     {
        JFreeChart chart = ChartFactory.createBarChart3D
        ("","Provider", "Number of Accesses",
        getDataSet(), PlotOrientation.VERTICAL, true,true, false);
        //chart.setBackgroundPaint(Color.gray);
        chart.setBackgroundPaint(new Color(249, 231, 236));
        chart.getTitle().setPaint(Color.blue);
        CategoryPlot p = chart.getCategoryPlot();
        p.setRangeGridlinePaint(Color.red);
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
