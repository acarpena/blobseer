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
import mon.util.db.*;

import java.util.*;
import javax.servlet.http.HttpSession;
import org.jfree.chart.entity.StandardEntityCollection;
import org.jfree.data.general.AbstractDataset;
import org.jfree.data.general.DatasetUtilities;
import javax.servlet.http.*;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.chart.axis.CategoryAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.data.xy.CategoryTableXYDataset;

/**
 *
 * @author tylor
 */
public class system_storage
{
    private double storage_limit_of_provider;
    private double unit;
    private String show_type; // system, provider
    public String getshow_type()
    {
        return show_type;
    }
    public void setshow_type(String type)
    {
        show_type=type;
    }
    public double getstorage_limit_of_provider()
    {
        return storage_limit_of_provider;
    }
    public void setstorage_limit_of_provider(double limit)
    {
        storage_limit_of_provider=limit*1024*1024*1024; //limit is based on GB
    }
    public void setunit(double un)
    {
        unit=un;
    }
    public double getunit()
    {
        return unit;
    }
    private  AbstractDataset getDataSet()
    {
    	AbstractDataset dataset = null;
        DatabaseFastDB db = new DatabaseFastDB();
        ArrayList node_list = new ArrayList();
        ArrayList blob_list = new ArrayList();
        String sqlcmd = "", sqlcmd2 = "";
        sqlcmd = "select blob_id from blobs order by blob_id asc";
    	db.executeQuery(sqlcmd);
    	// construct blob_list as well as node_list
    	sqlcmd="";
    	while(db.next())
    	{
    		blob_list.add(db.getString(1));
    		sqlcmd += "select distinct node from write_" + db.getString(1)+"  union ";
    	}
        sqlcmd = sqlcmd.substring(0,sqlcmd.length()-8);
        sqlcmd = "select distinct node from ("+sqlcmd+") as tmp order by  node asc";
        db.executeQuery(sqlcmd);
        System.out.println(sqlcmd);
        while(db.next())
        {
        	node_list.add(db.getString(1));
        }
        // show 2 types of charts using different types of datasets
        if(show_type.equals("system"))
        {
        	
        	sqlcmd = "";
        	for(int i=0; i<blob_list.size(); i++)
        	{
        		sqlcmd  += "select write_size from client_writes_"+blob_list.get(i)+" union ";
        	}
	        //get the sum up of all data provider's used space.
        	sqlcmd = sqlcmd.substring(0,sqlcmd.length()-8);
        	sqlcmd ="select sum(write_size) from ("+sqlcmd+") as tmp";
            db.executeQuery(sqlcmd);
            System.out.println(sqlcmd);   
            // show it
            dataset = new DefaultCategoryDataset();
            ((DefaultCategoryDataset) dataset).addValue(db.getDouble(1)/unit,"Used Storage","System");
            ((DefaultCategoryDataset) dataset).addValue((node_list.size()*storage_limit_of_provider-db.getDouble(1))/unit,
            "Available Storage","System");
        }
        else if(show_type.equals("provider"))
        {
        	dataset = new CategoryTableXYDataset();
            for(int i=0;i<node_list.size();i++)
	        {
            	sqlcmd ="";
	        	for(int j=0;j<blob_list.size();j++)
	        	{
	        		sqlcmd += "select count(*)*(select page_size from blobs where blob_id='"+
	        		(String)blob_list.get(j)+"') as num from write_"+(String)blob_list.get(j)+
	        		" where node='"+(String)node_list.get(i)+"'  union ";
	        		
	            }
	        	sqlcmd = sqlcmd.substring(0,sqlcmd.length()-8);
	        	sqlcmd = "select sum(num) from ( "+sqlcmd+" ) as tmp";
	        	db.executeQuery(sqlcmd);
	        	System.out.println(sqlcmd);
	        	((CategoryTableXYDataset) dataset).add(i, (storage_limit_of_provider-db.getDouble(1))/unit,"Available Storage");
	        	((CategoryTableXYDataset) dataset).add(i, db.getDouble(1)/unit,"Used Storage");
	        }
          }
        return dataset;
    }
    /*
    private  CategoryTableXYDataset getDataSet_1()
    {
        CategoryTableXYDataset dataset =  new CategoryTableXYDataset();
        DB db=new DB();
        if(show_type.equals("system"))
        {
          //  db.query("select sum(page_size),count(distinct node) from blob_pages");
          //  dataset.addValue(db.getd(1)/unit,"Used Storage","System");
          //  dataset.addValue((db.geti(2)*storage_limit_of_provider-db.getd(1))/unit,"Available Storage","System");
        }
        else if(show_type.equals("provider"))
        {
            int i=0;
            db.query("select sum(page_size), node from blob_pages group by node");
            while(db.moveNext())
            {
                dataset.add(i,db.getd(1)/unit,"Used Storage");
                dataset.add(i,(storage_limit_of_provider-db.getd(1))/unit,"Available Storage");
                i++;
            }
        }
        return dataset;
    }*/
     public String getChartViewer(HttpServletRequest request, HttpServletResponse response)
     {
       String unit_str="Size";
       if(unit==1024)
           unit_str="Size (KB)";
       else if(unit==1048576)
           unit_str="Size (MB)";
       else // if(unit==1)
           unit_str="Size (Byte)";
       JFreeChart chart =null;
       if(show_type.equals("system"))
       {
            chart = ChartFactory.createStackedBarChart3D(
                "", 
                show_type, 
                unit_str,
                (DefaultCategoryDataset)getDataSet(), 
                PlotOrientation.VERTICAL, 
                true, 
                true, 
                false);
       }
       else if(show_type.equals("provider"))
       {
           chart = ChartFactory.createStackedXYAreaChart(
            "",      // chart title
            show_type,                // domain axis label
            unit_str,                   // range axis label
            (CategoryTableXYDataset)getDataSet(),                   // data
            PlotOrientation.VERTICAL,  // orientation
            true,                      // include legend
            true,
            false);
       }
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
