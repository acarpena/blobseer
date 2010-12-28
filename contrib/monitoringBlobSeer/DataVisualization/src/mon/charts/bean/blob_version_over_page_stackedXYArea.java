package mon.charts.bean;
import java.awt.Color;
import java.awt.image.*;
import java.io.PrintWriter;
import java.util.ArrayList;
import org.jfree.chart.*;
import javax.servlet.http.*;
import lia.Monitor.Store.Fast.DB;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.entity.StandardEntityCollection;
import org.jfree.chart.plot.Plot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.xy.*;
/**
/**
 *s
 * @author tylor
 */
public class blob_version_over_page_stackedXYArea {
    public String blob_id;
    public String version_str;
    public int version;
    public String getblob_id()
    {
        return blob_id;
    }
    public void setblob_id(String id)
    {
        blob_id=id;
    }
    public String getversion()
    {
        return version_str;
    }
    public void setversion(String ver)
    {
        version_str=ver;
        version=Integer.parseInt(version_str);
    }
    private CategoryTableXYDataset getDataSet()
    {
        int num_of_points=1000;
        CategoryTableXYDataset dataset = new CategoryTableXYDataset();
        DB db=new DB();
        db.query("select count(distinct written_version),max(blob_offset) from blob_pages where blob_id="+blob_id);
        db.moveNext();
        int count_of_versions=db.geti(1);
        double offset_range=db.getd(2);
        double step=offset_range/num_of_points;
        //double step=5;
        if(version!=0)
        {
            //step_series=new XYSeries("version "+version);
        }
        double pos=0;
        double start_x = 0;
        double end_x =0;
        double v=0;
       // double check_point[]=new double[version];
        //int check_point_idx=0;
       // double stop_x=0;
        double ratio=1; ///////******************************
        for( v = 1; v <= count_of_versions; v++)
        {
            db.query("select min(blob_offset),max(blob_offset) from blob_pages" +
                    " where blob_id="+blob_id +"and written_version="+v);
            db.moveNext();
            start_x = db.getd(1);
            end_x = db.getd(2);
            System.out.println(" start: "+start_x+" end "+end_x);
            for(double x = start_x; x <= end_x-step; x+=step)
            {
                dataset.add(x,v,""+v);
            }
            dataset.add(end_x,v,""+v);
            
        }
        //////////////////////////////////////////////////////
        if(version != 0)
        {
            ArrayList start_point_list=new ArrayList();
            ArrayList version_list=new ArrayList();
            int cur_version=-1;
            String sqlcmd="select blob_offset , written_version from blob_pages " +
                    " where blob_id="+blob_id+" and written_version<="+version+
                    " and overwritten_version>"+version+" order by blob_offset asc";
            db.query(sqlcmd);
            db.moveNext();
            double previous_x=0;
            do
            {
          //      System.out.println("verion is:"+cur_version+" new is "+db.geti(2));
                if(db.geti(2) != cur_version)
                {
                    cur_version=db.geti(2);
                    start_point_list.add(""+db.getd(1));
                    version_list.add(""+cur_version);
            //        System.out.println("verion add:"+cur_version+" start_x add "+db.getd(1));
                }
                previous_x = db.getd(1);
            }while(db.moveNext());
            //System.out.println("Out of while");
            
            start_point_list.add(""+previous_x);
            version_list.add(""+cur_version);
            //System.out.println("verion add:"+cur_version+" start_x add "+previous_x);

            
            for(int i=0;i<=start_point_list.size()-2;i++)
            {
                cur_version = Integer.parseInt(((String)version_list.get(i)));
                start_x = Double.parseDouble(((String)start_point_list.get(i)));
                end_x = Double.parseDouble(((String)start_point_list.get(i+1)));
                double x_range = end_x - start_x;
                step = x_range / 100;
            //    System.out.println("start_point "+start_x+" end_x "+end_x+" step "+step+" ver:"+cur_version);
                
                for(double x = start_x; x <= end_x; x += step)
                {
                   // step_series.add(x,cur_version+0.15);
                }
                //System.out.println("OK "+i);
            }
           // coll.addSeries(step_series);
        }
       
        return dataset;
    }
    
    public String getChartViewer(HttpServletRequest request, HttpServletResponse response) {
        final JFreeChart chart = ChartFactory.createStackedXYAreaChart(
            "",      // chart title
            "X-Axis",                // domain axis label
            "version no.",                   // range axis label
            getDataSet(),                   // data
            PlotOrientation.VERTICAL,  // orientation
            true,                      // include legend
            true,
            false
        );
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
