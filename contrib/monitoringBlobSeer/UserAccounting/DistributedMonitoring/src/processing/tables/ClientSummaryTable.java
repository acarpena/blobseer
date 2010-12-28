package processing.tables;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Vector;

import util.TimeUtil;
import util.db.DatabaseAccess;
import data.ClientSummaryData;
import data.MonitorData;

/**
 * class that handles operating with the summary_table
 * @author Michi
 *
 */
public class ClientSummaryTable extends TableInterface
{
	public ClientSummaryTable(DatabaseAccess dba)
	{
		super("summary_table", dba);
	}

	@Override
	public void createTable(String str)
	{
		String sqlcmd="CREATE TABLE " + table 
		+ "("
		+ "client_id VARCHAR(255)," 
		+ "timestamp VARCHAR(255),"
		+ "blob_id VARCHAR(255),"
		+ "watermark VARCHAR(255),"
		+ "provider_pages BIGINT,"
		+ "write_size_vman VARCHAR(255),"
		+ "page_size BIGINT,"
		+ "PRIMARY KEY(client_id, blob_id, timestamp, watermark)"
		+ ")";
		db.executeQuery(sqlcmd);
	}

	@Override
	public void insert(MonitorData d)
	{
		ClientSummaryData data = (ClientSummaryData)d;

		String sqlcmd="INSERT INTO "+table
		+" (client_id,timestamp,blob_id,watermark, provider_pages,write_size_vman, page_size)"
		+"VALUES ("+format(data.clientID) +","+TimeUtil.getTimeMillis(data.timestamp)+","+ format(data.blob_id) +","
		+ format(data.watermark) +"," + data.provider_pages +","
		+ data.write_size_vman+","+data.page_size+")";

		db.executeQuery(sqlcmd);
	}

	public Vector<ClientSummaryData> getData(String timestamp_val)
	{
		Vector<ClientSummaryData> data_container = new Vector<ClientSummaryData>();
		if (this.equals(null))
    		return null;
    	//TODO modificat asta!!!!!!!!
    	String sqlcmd = "SELECT * FROM "+ table + " WHERE timestamp > '"+ TimeUtil.getTimeMillis(timestamp_val)+"'";
    	ResultSet rs = db.query(sqlcmd);
    	try
		{
			while (rs.next())
			{
				ClientSummaryData data = new ClientSummaryData();
				data.clientID = rs.getString(1);
				data.timestamp = rs.getString(2);
				data.blob_id = rs.getString(3);
				data.provider_pages = rs.getInt(5);
				data.watermark = rs.getString(4);
				data.write_size_vman = rs.getLong(6);
				data.page_size = rs.getInt(7);
								
				data_container.add(data);
			}
		} catch (SQLException e)
		{
			e.printStackTrace();
		}
		
		return data_container;
	}
}
