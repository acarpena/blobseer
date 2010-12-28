function showMenu()
{
		d = new dTree('d');
		d.add(0,-1,'MonALISA Repository','show?page=index.html');
		//d.add(1,0,'Global Views','');
		//d.add(2,0,'Statistics','');
		//d.add(11,0,'Masters','display?page=spider');
		//d.add(10,0,'Sites Status','stats?page=summary');
		//d.add(5,0,'Site Info','info.jsp');
	
		d.add(101,0,'BlobSeer Monitoring','');
                d.add(1011,101,'General Parameters','');
                d.add(1012,101,'Specialized Parameters','');
                d.add(10111,1011,'Node CPU Usage','display?page=UserCPU');
                d.add(10112,1011,'Node Memory Usage','display?page=UsedMem');
                d.add(10113,1011,'Network Traffic (inward)','display?page=eth_in');
                d.add(10122,1012,'Blob I/O View','');
                d.add(101223,10122,'I/O distribution over blob','io_over_blob_select.jsp');
                d.add(101224,10122,'Blob I/O distribution over provider overview','blob_io_over_provider.jsp');
                d.add(101225,10122,'Blob I/O distribution over proviser detailed view','blob_io_sortby_provider_select.jsp');
                d.add(101226,10122,'No. of Access per version','no_of_access_per_version_select.jsp');
                d.add(101227,10122,'Node Memory Stacked XY Area','memory_stackedXYArea.jsp');

                d.add(10121,1012,'Blob Storage View','');
                d.add(101211,10121,'Blob Storage over version (Stacked Bar)','blob_storage_over_version_stackedBar_select.jsp')
                d.add(101212,10121,'Blob Storage over version (Stacked XY Area)','blob_storage_over_version_stackedXYArea_select.jsp')
		d.add(101213,10121,'Blob Storage over version (Bar, Single Blob)','blob_storage_over_version_bar_select.jsp')
                d.add(101214,10121,'Provider Storage over blob','provider_storage_over_blob_select.jsp')
                d.add(101215,10121,'Blob Version over Page ID','blob_version_over_page_select.jsp')
                d.add(101216,10121,'Blob Most Accessed Pages','most_accessed_page_select.jsp')
                d.add(101217,10121,'System Storage','system_storage_select.jsp')
                d.add(101218,10121,'provider storage for specific blob','provider_storage_for_specific_blob_select.jsp')
	
		d.add(103,2,'Blob_I/O overview using Properties files','stats?page=blob_IO');
		d.add(106,2,'Farms','stats?page=stats2_2');
		d.add(107,2,'Traffic','stats?page=stats2_3');
		d.add(109,2,'Farms Load','stats?page=stats_usage');
		
		d.add(113,1,'Farms Usage','display?page=spider_usage');
		d.add(12,11,'Load','');
		d.add(13,12,'Real-Time','display?page=rt_link2');
		d.add(14,12,'History','display?page=hist_link2');
		d.add(15,11,'Traffic','display?page=eth');
		d.add(16,15,'eth0','');
			
		d.add(6,1,'Interactive Map','show?page=index.html');
		d.add(108,1,'Load 5 - Spider','display?page=spider')
		
		d.add(1015,101,'Client Accesses Monitoring','');
                //,'Node CPU Usage','display?page=UserCPU');
                //d.add(10112,1011,'Node Memory Usage','display?page=UsedMem');
                //d.add(10113,1011,'Network Traffic (inward)','display?page=eth_in');
                //d.add(10122,1012,'Blob I/O View','');
                d.add(101523,1015,'Client I/O distribution over blob','user_accounting/NumberOfClientsPerBlob.jsp');
                //d.add(101524,1015,'Client Blob I/O distribution over provider overview','clientAccesses/blob_io_over_provider.jsp');
                //d.add(101525,1015,'Blob I/O distribution over proviser detailed view','blob_io_sortby_provider_select.jsp');
                //d.add(101526,1015,'No. of Access per version','clientAccesses/no_of_access_per_version_select.jsp');
                //d.add(101527,1015,'Node Memory Stacked XY Area','memory_stackedXYArea.jsp');



		document.write(d);
}
				
