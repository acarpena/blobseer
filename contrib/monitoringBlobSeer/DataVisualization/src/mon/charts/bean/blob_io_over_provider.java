/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package mon.charts.bean;
/**
 *
 * @author tylor
 */
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
import org.jfree.chart.urls.StandardCategoryURLGenerator;
import org.jfree.data.category.DefaultCategoryDataset;

public class blob_io_over_provider
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
	  /*
	  private HashMap<String, Double> getDataSetOneBlob(String blob_id, DatabaseFastDB db)
      {
		  Blobs blobs = new Blobs(db);
		  double page_size = blobs.getBlobPageSize(blob_id);
		  HashMap<String, Double> blob_io = new HashMap<String, Double>();		  
          if(metric.equals("num_of_access"))
          {
        	  db.executeQuery("select provider , count(page_index)  from write_" 
                      + blob_id
                      + " group by provider"
                      + " order by provider");

           }
          else if(metric.equals("total_size"))
          {
        	  db.executeQuery("select provider, count(page_index)*"+ page_size+
                    		  " from client_writes_"+ blob_id  
                                      + " group by client_id "
                                      + " order by client_id");
           }

          while(db.next())
          {
             blob_io.put(db.getString(1), db.getDouble(2));
          }

              return blob_io;
      }
      private HashMap<String, Double> getDataSetAllBlobs(DatabaseFastDB db)
      {
    	  HashMap<String, Double> all_blob_io = new HashMap<String, Double>(); 
          Blobs blobs = new Blobs(db);
          ArrayList<String> blob_list = blobs.getBlobIDs();
          for (int i=0; i<blob_list.size(); i++)
          {
        	  String blob_id = blob_list.get(i);
              HashMap<String, Double> single_blob_io = this.getDataSetOneBlob(blob_id, db);
              Set<String> l = single_blob_io.keySet();
              for (Iterator<String> it = l.iterator(); it.hasNext();)
              {
            	  String node = it.next();
            	  Double value = all_blob_io.get(cl);
            	  if (value != null)
            	  {
            		  all_blob_io.put(cl, value + single_blob_io.get(cl));
                  }
                  else
                  {
                	  all_blob_io.put(cl, single_blob_io.get(cl));
                  }
                }         
          }
          return all_blob_io;
      }
*/
      private  CategoryDataset getDataSet()
     {
    	  this.metric="total_size";
        DefaultCategoryDataset dataset =  new DefaultCategoryDataset();
        DatabaseFastDB db=new DatabaseFastDB();
        Blobs blobs = new Blobs(db);
        ArrayList blob_list = blobs.getBlobIDs();
        String sqlcmd="";
        
        for(int i=0; i<blob_list.size(); i++)
        {	
        	String metric_sql="count(page_index)";
        	if(metric.equals("total_size"))
            {
        		double page_size=blobs.getBlobInfos().get(i).getPageSize();
            	metric_sql = metric_sql+"*"+page_size;
            }
        	sqlcmd="select node, "+ metric_sql +" from write_"+blob_list.get(i)+
        	" group by node order by node asc";
        	//System.out.println(sqlcmd);
        	db.executeQuery(sqlcmd);
        	while(db.next())
        	{
        		dataset.addValue(db.getDouble(2), db.getString(1), "blob "+blob_list.get(i));
        	}
        }
        return dataset;
    }
    public String getChartViewer(HttpServletRequest request, HttpServletResponse response) {
       JFreeChart chart = ChartFactory.createStackedBarChart3D(
                "", "Blob", "Blob Read & Write Accesses over Providers",
                getDataSet(), PlotOrientation.VERTICAL, true, true, false);
        chart.setBackgroundPaint(new Color(249, 231, 236));
        CategoryPlot plot = chart.getCategoryPlot();
        plot.setNoDataMessage("No data available");
        //plot.setLabelGenerator(null);

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
          BufferedImage chartImage = chart.createBufferedImage(600,1400, info);
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
