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

import java.util.*;

import javax.servlet.http.HttpSession;
import org.jfree.chart.entity.StandardEntityCollection;
import org.jfree.data.general.AbstractDataset;
import org.jfree.data.general.DatasetUtilities;
import org.jfree.data.xy.CategoryTableXYDataset;

import javax.servlet.http.*;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.chart.axis.CategoryAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.data.*;


/**
 *
 * @author tylor
 */
public class provider_storage_over_blob
{
	private String chart_type;
	public String getchart_type()
	{
		return chart_type;
	}
	public void setchart_type(String type)
	{
		chart_type=type;
	}
	 private  CategoryTableXYDataset getDataSet_1()
	 {
	        CategoryTableXYDataset dataset =  new CategoryTableXYDataset();
	        ArrayList node_list=new ArrayList();
	        ArrayList blob_list=new ArrayList();
	        DatabaseFastDB db=new DatabaseFastDB();
	        db.executeQuery("select distinct blob_id from blobs order by blob_id asc");
	        String sqlcmd="";
	        while(db.next())
	        {
	           blob_list.add(db.getString(1));
	           sqlcmd +=" select distinct node from write_" + db.getString(1)+"  union ";
	        }
	        sqlcmd = sqlcmd.substring(0,sqlcmd.length()-8);
	        sqlcmd = "select distinct node from ( "+sqlcmd+" ) as tmp order by node asc";
	        db.executeQuery(sqlcmd);
	       // System.out.println(sqlcmd);
	        while(db.next())
	        {
	            node_list.add(""+db.getString(1));
	        }
	        for(int i=0;i<node_list.size();i++)
	        {
	           
	        	for(int j=0;j<blob_list.size();j++)
	        	{
	        		sqlcmd = "select count(*)*(select page_size from blobs where blob_id='"+
	        		(String)blob_list.get(j)+"') from write_"+(String)blob_list.get(j)+
	        		" where node='"+(String)node_list.get(i)+"'";
	        		db.executeQuery(sqlcmd);
	        		//System.out.println(sqlcmd);
	        		dataset.add(i,db.getDouble(1), "Blob["+blob_list.get(j)+"]");
	            }
	        }
	   return dataset;
	}
	 private  CategoryDataset getDataSet()
	 {
	        DefaultCategoryDataset dataset =  new DefaultCategoryDataset();
	        ArrayList node_list=new ArrayList();
	        ArrayList blob_list=new ArrayList();
	        DatabaseFastDB db=new DatabaseFastDB();
	        db.executeQuery("select distinct blob_id from blobs order by blob_id asc");
	        String sqlcmd="";
	        while(db.next())
	        {
	           blob_list.add(db.getString(1));
	           sqlcmd +=" select distinct node from write_" + db.getString(1)+"  union ";
	        }
	        sqlcmd = sqlcmd.substring(0,sqlcmd.length()-8);
	        sqlcmd = "select distinct node from ( "+sqlcmd+" ) as tmp order by node asc";
	        db.executeQuery(sqlcmd);
	        System.out.println(sqlcmd);
	        while(db.next())
	        {
	            node_list.add(""+db.getString(1));
	        }
	        for(int i=0;i<node_list.size();i++)
	        {
	           
	        	for(int j=0;j<blob_list.size();j++)
	        	{
	        		sqlcmd = "select count(*)*(select page_size from blobs where blob_id='"+
	        		(String)blob_list.get(j)+"') from write_"+(String)blob_list.get(j)+
	        		" where node='"+(String)node_list.get(i)+"'";
	        		db.executeQuery(sqlcmd);
	        		System.out.println(sqlcmd);
	        		dataset.addValue(db.getDouble(1),"Blob["+blob_list.get(j)+"]",""+i);
	        	}
	        }
	        return dataset;
	}
    public String getChartViewer(HttpServletRequest request, HttpServletResponse response)
     {
    	JFreeChart chart=null;
    	if(chart_type.equals("xyAreaChart"))
    	{
    		chart = ChartFactory.createStackedXYAreaChart(
    	            "",      // chart title
    	            "Provider",                // domain axis label
    	            "",                   // range axis label
    	            getDataSet_1(),                   // data
    	            PlotOrientation.VERTICAL,  // orientation
    	            true,                      // include legend
    	            true,
    	            false);
    	}
    	else  //  (chart_type.equals("barChart"))
    	{
    		chart = ChartFactory.createStackedBarChart3D(
                    "", 
                    "Provider", 
                    "",
                    getDataSet(), 
                    PlotOrientation.HORIZONTAL, 
                    true, 
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
