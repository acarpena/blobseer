package mon.charts.bean;
import java.io.*;
import java.awt.*;
import java.util.ArrayList;
import java.awt.image.*;
import org.jfree.chart.*;
import org.jfree.chart.plot.*;
import org.jfree.chart.urls.*;
import org.jfree.chart.annotations.CategoryTextAnnotation;
import org.jfree.chart.annotations.TextAnnotation;
import org.jfree.chart.entity.*;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.data.general.DefaultPieDataset;
import javax.servlet.http.*;
import lia.Monitor.Store.Fast.DB;
public class io_over_blob 
{
	  private String metric; //num_of_access, total_size
	  private double unit;   //byte, KB, MB
	  private int operation; // 0=read, write=1, read & write=2
	  public void setoperation(int op)
	  {
		  operation=op;
	  }
	  public int getoperation()
	  {
		  return operation;
	  }
	  public void setunit(double u)
	  {
		  unit=u;
	  }
	  public double getunit()
	  {
		  return unit;
	  }
	  public void setmetric(String m)
	  {
		  metric = m;
	  }
	  public String getmetric()
	  {
		  return metric;
	  }
      public io_over_blob() {}

      private DefaultPieDataset getDataset()
      {
    	//long start = System.currentTimeMillis();
        DB db=new DB();
        db.query("select distinct blob_id from blobs order by blob_id asc");
        ArrayList blob_list=new ArrayList();
        DefaultPieDataset dataset=new DefaultPieDataset();
        while(db.moveNext())
        {
            blob_list.add(db.gets("blob_id"));
        }
        //double[] data=new double[blob_list.size()];
        String sqlcmd=null;
        for(int i=0;i<blob_list.size();i++)
        {
        	if(metric.equals("num_of_access"))
            	sqlcmd="select count(timestamp) from client_writes_";
            else if(metric.equals("total_size"))
            	sqlcmd="select sum(write_size)/"+unit+" from client_writes_";
        	sqlcmd+=blob_list.get(i);
        	/*
        	if(operation == 0 || operation == 1)
        	{
        		
        	}*/
        	//System.out.println(sqlcmd);
            db.query(sqlcmd);
            db.moveNext();
            dataset.setValue("Blob "+(String)blob_list.get(i),db.getd(1));
        }
        //long duration=System.currentTimeMillis()-start;
        //System.out.println("NEW--->"+duration);
        return dataset;
      }

      public String getChartViewer(HttpServletRequest request, HttpServletResponse response) {
        DefaultPieDataset dataset = getDataset();
        // create the chart...
        JFreeChart chart = ChartFactory.createPieChart3D(
              "",  // chart title
              dataset,             // data
              true,                // include legend
              true,
              false
        );

        // set the background color for the chart...
        chart.setBackgroundPaint(new Color(249, 231, 236));
        PiePlot plot = (PiePlot) chart.getPlot();
        plot.setNoDataMessage("No data available");
        TextAnnotation anno = new CategoryTextAnnotation("Metric","",1);
        //plot.addAnnotation(anno);
        
        // set drilldown capability...
        plot.setURLGenerator(new StandardPieURLGenerator("blob_io_over_page.jsp","blob_id"));
        plot.setLabelGenerator(null);

        // OPTIONAL CUSTOMISATION COMPLETED.

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
