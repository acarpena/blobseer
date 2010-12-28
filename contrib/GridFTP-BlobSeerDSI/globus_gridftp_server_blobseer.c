/*
 * Copyright 1999-2006 University of Chicago
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * BLOBSEER DSI for GRIDFTP
 * Author : vdinh@irisa.fr
 */

#include "globus_gridftp_server.h"
#include "globus_gridftp_server_blobseer.h"
#include "NamespaceClientC.h"
#include "globus_common.h"

#define GLOBUS_BLOBSEER_CONFIG_FILE "/etc/blobseer.cfg"

/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 *  ADD ADDITIONAL INCLUDES HERE
 *@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/

GlobusDebugDefine(GLOBUS_GRIDFTP_SERVER_BLOBSEER);

static 	int 	blobseer_l_dev_wrapper = 10;
static  globus_size_t 	pagesize = 0;
static  char*	blobseer_config_string = NULL;

static
globus_version_t local_version =
{
    0, /* major version number */
    1, /* minor version number */
    1267094624,
    0 /* branch ID */
};

typedef struct globus_l_gfs_blobseer_handle_s
{
    int                                 some_needed_data;
} globus_l_gfs_blobseer_handle_t;

/*
 * @vdinh : create a new struct that can help to monitor the status of read/write
 */
typedef struct
{
	globus_gfs_operation_t 	op; 	    	//operation
	globus_mutex_t 			lock;			 //for paralellism
	globus_mutex_t			blob_lock;


	globus_size_t 			block_size; 	//block_size
	int						optimal_count;  //number of paralellism

	globus_memory_t 		mem;	//memory for buffer in server side
	globus_list_t*			buffer_list;

	char* 					pathname;
	globus_off_t			file_offset;
	globus_bool_t			eof;
	globus_bool_t			first_read;
	globus_bool_t			first_write;
	globus_bool_t			finish;
	globus_bool_t			register_end;

	int 					pending_reads;
	int 					pending_writes;

	int 					number_reads_from_net;
	int						number_write_blob;

	int 					number_writes_to_net;
	int						number_read_blob;
	globus_off_t 			read_length;
	globus_off_t 			read_offset;

	globus_priority_q_t		queue;
	globus_object_t *		error;

	blob_env_t *			env;
	ns_handler_t * 			ns;
	fblob_t * 				file_handler;
	int						freq;




}globus_l_gfs_blobseer_monitor_t;

typedef struct
{
	globus_byte_t * buffer;
	globus_off_t 	offset;
	globus_size_t	length;
} globus_l_gfs_blobseer_buffer_info_t ;



int
globus_l_gfs_blobseer_queue_compare(
    void *                              priority_1,
    void *                              priority_2)
{
    globus_l_gfs_blobseer_buffer_info_t *            buf_info1;
    globus_l_gfs_blobseer_buffer_info_t *            buf_info2;
    int                                 rc = 0;
    GlobusGFSName(globus_l_gfs_hpss_local_queue_compare);
    GlobusGFSBlobseerDebugEnter();

    buf_info1 = (globus_l_gfs_blobseer_buffer_info_t *) priority_1;
    buf_info2 = (globus_l_gfs_blobseer_buffer_info_t *) priority_2;

    /* the void * are really just offsets */
    if(buf_info1->offset > buf_info2->offset)
    {
        rc = 1;
    }
    if(buf_info1->offset < buf_info2->offset)
    {
        rc = -1;
    }

    GlobusGFSBlobseerDebugExit();
    return rc;
}

/*************************************************************************
 *  start
 *  -----
 *  This function is called when a new session is initialized, ie a user 
 *  connectes to the server.  This hook gives the dsi an oppertunity to
 *  set internal state that will be threaded through to all other
 *  function calls associated with this session.  And an oppertunity to
 *  reject the user.
 *
 *  finished_info.info.session.session_arg should be set to an DSI
 *  defined data structure.  This pointer will be passed as the void *
 *  user_arg parameter to all other interface functions.
 * 
 *  NOTE: at nice wrapper function should exist that hides the details 
 *        of the finished_info structure, but it currently does not.  
 *        The DSI developer should jsut follow this template for now
 ************************************************************************/
/*
 * @vdinh : This get calls by the server after when the user attemp login,
 * after the grid-map lookup
 * In this funtion we can check and init the session.
 * If there is an failure, the client will be rejected.
 */
static
void
globus_l_gfs_blobseer_start(
    globus_gfs_operation_t              op,
    globus_gfs_session_info_t *         session_info)
{

    globus_l_gfs_blobseer_handle_t *       blobseer_handle;
    globus_gfs_finished_info_t          finished_info;


    GlobusGFSName(globus_l_gfs_blobseer_start);
    GlobusGFSBlobseerDebugEnter();

    globus_gfs_log_message(
        GLOBUS_GFS_LOG_INFO,
        "Blobseer start function...\n");

    blobseer_handle = (globus_l_gfs_blobseer_handle_t *)
        globus_malloc(sizeof(globus_l_gfs_blobseer_handle_t));

    blobseer_handle->some_needed_data = 0;
    memset(&finished_info, '\0', sizeof(globus_gfs_finished_info_t));
    finished_info.type = GLOBUS_GFS_OP_SESSION_START;
    finished_info.result = GLOBUS_SUCCESS;
    finished_info.info.session.session_arg = blobseer_handle;
    finished_info.info.session.username = session_info->username;
    finished_info.info.session.home_dir = "/";

    //@vdinh: if there is no errors, normaly we return the finished function
    globus_gridftp_server_operation_finished(
        op, GLOBUS_SUCCESS, &finished_info);

    GlobusGFSBlobseerDebugExit();
    return 0;
}

/*************************************************************************
 *  destroy
 *  -------
 *  This is called when a session ends, ie client quits or disconnects.
 *  The dsi should clean up all memory they associated wit the session
 *  here. 
 ************************************************************************/
static
void
globus_l_gfs_blobseer_destroy(
    void *                              user_arg)
{

    globus_gfs_log_message(
        GLOBUS_GFS_LOG_INFO,
        "Blobseer destroy function...\n");
    globus_l_gfs_blobseer_handle_t *       blobseer_handle;

    blobseer_handle = (globus_l_gfs_blobseer_handle_t *) user_arg;

    globus_free(blobseer_handle);
}

/*
 * Get information about file attribute
 */
static
globus_result_t
blob_stat(char * pathname, globus_gfs_stat_t * stat_out)
{
	stat_out->symlink_target = NULL;
	stat_out->name = strdup(pathname);
	stat_out->nlink = 0;
	stat_out->uid = 0;
	stat_out->gid = 0;
	stat_out->size = 0;

	stat_out->ctime = 0;
	stat_out->mtime = 0;
	stat_out->atime = 0;

	stat_out->mode = S_IFREG | S_IRUSR | S_IWUSR |
					S_IXUSR | S_IXOTH | S_IRGRP | S_IXGRP;

	stat_out->dev = blobseer_l_dev_wrapper ++;
	stat_out->ino = 0;
}

/*************************************************************************
 *  stat
 *  ----
 *  This interface function is called whenever the server needs
 *  information about a given file or resource.  It is called then an
 *  LIST is sent by the client, when the server needs to verify that
 *  a file exists and has the proper permissions, etc.
 ************************************************************************/


static
void
globus_l_gfs_blobseer_stat(
    globus_gfs_operation_t              op,
    globus_gfs_stat_info_t *            stat_info,
    void *                              user_arg)
{
    globus_gfs_stat_t *	                stat_array;
    int                                 stat_count = 1;
    globus_gfs_stat_t					stat_buf;

    GlobusGFSName(globus_l_gfs_blobseer_stat);
    GlobusGFSBlobseerDebugEnter();

    globus_gfs_log_message(
	        GLOBUS_GFS_LOG_INFO,
	        "Blobseer stat function...\n");

    // get file information
    blob_stat(stat_info->pathname,&stat_buf);
    if (!S_ISDIR(stat_buf.mode) || stat_info->file_only )
    {
    	//if it is a file
    	stat_array = (globus_gfs_stat_t *) globus_calloc(1,sizeof(globus_gfs_stat_t));
    	memcpy(stat_array,&stat_buf,sizeof(globus_gfs_stat_t));
    }
    else
    {
    	//it is a dir
    	//LISTING a directory files
    }
    globus_gridftp_server_finished_stat(
        op, GLOBUS_SUCCESS, stat_array, stat_count);
    GlobusGFSBlobseerDebugExit();
    return;
}

/*************************************************************************
 *  command
 *  -------
 *  This interface function is called when the client sends a 'command'.
 *  commands are such things as mkdir, remdir, delete.  The complete
 *  enumeration is below.
 *
 *  To determine which command is being requested look at:
 *      cmd_info->command
 *
 *      GLOBUS_GFS_CMD_MKD = 1,
 *      GLOBUS_GFS_CMD_RMD,
 *      GLOBUS_GFS_CMD_DELE,
 *      GLOBUS_GFS_CMD_RNTO,
 *      GLOBUS_GFS_CMD_RNFR,
 *      GLOBUS_GFS_CMD_CKSM,
 *      GLOBUS_GFS_CMD_SITE_CHMOD,
 *      GLOBUS_GFS_CMD_SITE_DSI
 ************************************************************************/
static
void
globus_l_gfs_blobseer_command(
    globus_gfs_operation_t              op,
    globus_gfs_command_info_t *         cmd_info,
    void *                              user_arg)
{
    globus_l_gfs_blobseer_handle_t *       blobseer_handle;
    GlobusGFSName(globus_l_gfs_blobseer_command);

    blobseer_handle = (globus_l_gfs_blobseer_handle_t *) user_arg;

    globus_gridftp_server_finished_command(op, GLOBUS_SUCCESS, GLOBUS_NULL);
}

/**********************
 * INITIALIZATION
 ***********************/
static
globus_result_t
globus_l_gfs_blobseer_monitor_init(
		globus_l_gfs_blobseer_monitor_t ** 	u_monitor,
		globus_size_t 						block_size,
		int 								optimal_count
		)
{
	globus_l_gfs_blobseer_monitor_t * monitor;
	globus_bool_t				  rc;

	GlobusGFSName(globus_l_gfs_hpss_local_monitor_init);
	GlobusGFSBlobseerDebugEnter();

	//allocate size for monitor value
	monitor = (globus_l_gfs_blobseer_monitor_t *)
			globus_malloc(sizeof(globus_l_gfs_blobseer_monitor_t));

	/*
	 * Allocate the buffer
	 * @mem_info : memory management datatype
	 * @block_size : the size of memory each nodes
	 * @count :the number of nodes
	 */
	rc = globus_memory_init(&monitor->mem, block_size, optimal_count);

	//puts("before memory init");
//	printf("Optimal couint is %d", optimal_count);
	globus_mutex_init(&monitor->lock,NULL);
	globus_mutex_init(&monitor->blob_lock,NULL);

	monitor->op = NULL;
	monitor->block_size = block_size;
	monitor->optimal_count = optimal_count;

	monitor->buffer_list = NULL;

	monitor->file_offset = 0;
	monitor->file_handler = NULL;
	monitor->eof = GLOBUS_FALSE;
	monitor->pending_reads = 0;
	monitor->pending_writes = 0;
	monitor->read_length = -1;
	monitor->read_offset =0;
	monitor->error = NULL;
	monitor->ns = NULL;
	monitor->first_read = GLOBUS_TRUE;
	monitor->first_write = GLOBUS_TRUE;

	monitor->finish = GLOBUS_FALSE;
	monitor->register_end = GLOBUS_FALSE;
	monitor->freq = 0;

	monitor->number_reads_from_net = 0;
	monitor->number_write_blob = 0;

	monitor->number_read_blob = 0;
	monitor->number_writes_to_net = 0;
	//get the configuration file and configuration parameters

	globus_gridftp_server_get_config_string(monitor->op,&blobseer_config_string);
	if (blobseer_config_string == NULL)
	{
		blobseer_config_string = GLOBUS_BLOBSEER_CONFIG_FILE;
	}
	//init the queue for store the buffer in the STORE scenario
	globus_priority_q_init(&monitor->queue,globus_l_gfs_blobseer_queue_compare);

	*u_monitor = monitor; //point to monitor value. This one is very important to reduce the segment fault error

	GlobusGFSBlobseerDebugExit();
	return GLOBUS_SUCCESS;// if there is no errors
}

static
void
globus_l_gfs_blobseer_monitor_destroy(
		globus_l_gfs_blobseer_monitor_t* monitor)
{
	globus_l_gfs_blobseer_buffer_info_t * buf_info;
	globus_list_t* 							list;
	globus_byte_t* 		buffer;
	GlobusGFSName(globus_l_gfs_blobseer_monitor_destroy);
	GlobusGFSBlobseerDebugEnter();

	while (!globus_priority_q_empty(&monitor->queue))
	{
		buf_info = (globus_l_gfs_blobseer_buffer_info_t * )
				globus_priority_q_dequeue(&monitor->queue);
		if (buf_info)
		{
			if (buf_info->buffer)
			{
				//free
				globus_memory_push_node(&monitor->mem,buf_info->buffer);
			}
			//free
			globus_free(buf_info);
		}
	}
	for(list = monitor->buffer_list;
	       !globus_list_empty(list);
	       list = globus_list_rest(list))
	     {
	        buffer = (globus_byte_t *) globus_list_first(list);
	        globus_memory_push_node(&monitor->mem, buffer);
	     };
	globus_priority_q_destroy(&monitor->queue);
	globus_memory_destroy(&monitor->mem);
	globus_mutex_destroy(&monitor->lock);
	globus_mutex_destroy(&monitor->blob_lock);
	globus_free(monitor);
	GlobusGFSBlobseerDebugExit();
}
/**************************************************
 * BLOBSEER FILE HANDLE
 **************************************************/

static
globus_result_t
globus_l_gfs_blobseer_file_init(globus_l_gfs_blobseer_monitor_t * monitor)
{

	globus_result_t result;
	GlobusGFSName(globus_l_gfs_blobseer_file_init);
	GlobusGFSBlobseerDebugEnter();

	monitor->env = (blob_env_t * ) malloc (sizeof(blob_env_t));
	monitor->ns = (ns_handler_t *) malloc (sizeof(ns_handler_t));
	globus_gfs_log_message(
					        GLOBUS_GFS_LOG_INFO,
					        "Blobseer : file initialize\n");
	result = ns_init(blobseer_config_string,monitor->env);
	if (result != 1)
	{
		globus_gfs_log_message(GLOBUS_GFS_LOG_ERR,"Blobseer configuration file error\n");
		goto error_init;
	}
	result = ns_initializeNamespaceHandler(monitor->env,monitor->ns);
	if (result != 1 )
	{
		globus_gfs_log_message(GLOBUS_GFS_LOG_ERR, "Blobseer namespace error ");
		goto error_namespace;
	}
	GlobusGFSBlobseerDebugExit();
	return result;
error_init :
error_namespace:
	GlobusGFSBlobseerDebugExitWithError();
	return result;
}

static
globus_result_t
globus_l_gfs_blobseer_file_finalize(
		void * user_arg)
{
	globus_l_gfs_blobseer_monitor_t * monitor;
	globus_result_t result;

	GlobusGFSName(globus_l_gfs_blobseer_file_finalize);
	GlobusGFSDebugEnter();
	globus_gfs_log_message(
		        GLOBUS_GFS_LOG_INFO,
		        "Globus_l_gfs_blobseer_file_finialize\n");
	monitor = (globus_l_gfs_blobseer_monitor_t *) user_arg;

	puts("before destroy");
	if (monitor->file_handler != GLOBUS_NULL)
		ns_destroyFileHandler(monitor->ns,monitor->file_handler);
	puts("after destroy");
	ns_free(monitor->env,monitor->ns);
	ns_finalize(monitor->env);
	GlobusGFSBlobseerDebugExit();
	return 0;
}

static
globus_result_t
globus_l_gfs_blobseer_file_open(const char* pathname,
						void * user_arg)
{
	globus_l_gfs_blobseer_monitor_t * monitor;
	u_int8_t	result;

	GlobusGFSName(globus_l_gfs_blobseer_file_open);
	GlobusGFSBlobseerDebugEnter();

	monitor = (globus_l_gfs_blobseer_monitor_t *) user_arg;
	monitor->file_handler = (fblob_t *) malloc (sizeof(fblob_t));
	monitor->file_handler = GLOBUS_NULL;

	ns_exists(monitor->ns,pathname,&result);
	if (result != 1) //file is not exist
	{
		globus_gfs_log_message(GLOBUS_GFS_LOG_ERR," file not found ");
		return GLOBUS_FALSE;
	}
	monitor->file_handler = ns_getFileHandler(monitor->ns,pathname);
	int version = fblob_getlatest(monitor->file_handler);
	monitor->read_length = fblob_getsize(monitor->file_handler,version);
	GlobusGFSBlobseerDebugExit();
	return GLOBUS_TRUE;
}

static
globus_result_t
globus_l_gfs_blobseer_file_create(const char* pathname,
						void * user_arg)
{
	globus_l_gfs_blobseer_monitor_t * monitor;
	u_int8_t	result;

	GlobusGFSName(globus_l_gfs_blobseer_file_create);
	GlobusGFSBlobseerDebugEnter();

	monitor = (globus_l_gfs_blobseer_monitor_t *) user_arg;

	monitor->file_handler = (fblob_t *) malloc (sizeof(fblob_t));
	ns_exists(monitor->ns,pathname,&result);
	if (result == 1) //file is exist
	{
		//delete the existing file
		ns_deleteFile(monitor->ns,pathname);
		globus_gfs_log_message(
				        GLOBUS_GFS_LOG_INFO,
				        "Blobseer : overwrite the existing file\n");
	}
	//create blobseer blob with the page size is equal to block_size
	globus_size_t page_size = monitor->block_size;
	monitor->file_handler = ns_createFile(monitor->ns,pathname,page_size,1);

	globus_gfs_log_message(GLOBUS_GFS_LOG_INFO,"blobseer file is created\n");
	GlobusGFSBlobseerDebugExit();
	return result;
}
static
globus_result_t
globus_l_gfs_blobseer_file_read(globus_l_gfs_blobseer_monitor_t *           monitor,
		                        globus_off_t offset, globus_off_t nbytes,char * buffer)
{
	globus_result_t result;
	GlobusGFSName(globus_l_gfs_blobseer_file_read);
	GlobusGFSBlobseerDebugEnter();
	int version = fblob_getlatest(monitor->file_handler);
	result = fblob_read(monitor->file_handler,version,offset,nbytes,buffer);
    GlobusGFSBlobseerDebugExit();
    return result;
}

static
globus_result_t
globus_l_gfs_blobseer_file_write(globus_l_gfs_blobseer_monitor_t * monitor,
								globus_off_t offset,globus_off_t length,char * buffer)
{
	globus_result_t result;
	GlobusGFSName(globus_l_gfs_blobseer_file_write);
	GlobusGFSBlobseerDebugEnter();
	/*
	 * Because the page_size is equal to block_size;
	 * and the written length is always equal or smaller than block_size
	 */
	//case 1 : length = page_size = block_size

	if (length == monitor->block_size)
	{
		result = fblob_write(monitor->file_handler,offset,length,buffer);
	}
	else  //length < page_size = block_size ; no greater case
	{
		int i;
		for (i = length +1; i < monitor->block_size; i++)
		{
			buffer[i] = '\0';
		}
		result = fblob_write(monitor->file_handler,offset,monitor->block_size,buffer);
	}
	GlobusGFSBlobseerDebugExit();
	return result;
}
/**************************************************
 * END: BLOBSEER FILE HANDLE
 **************************************************/





/*
 ******************************FOR STOR FUNCTION **************
 *Sending data from client to server
 *client : read data from local file storage
 *Transfer
 *server : write to blobseer
 */

// PROTOTYPE FUNCTION
static
void
globus_l_gfs_blobseer_server_read_cb(
    globus_gfs_operation_t   op,
    globus_result_t                     result,
    globus_byte_t *                     buffer,
    globus_size_t                       nbytes,
    globus_off_t                        offset,
    globus_bool_t                       eof,
    void *                              user_arg);

void
globus_l_gfs_blobseer_writeinto_storage_callback(
		globus_l_gfs_blobseer_monitor_t *           monitor);


static
globus_result_t
globus_l_gfs_blobseer_dispatch_write(
    globus_l_gfs_blobseer_monitor_t *       monitor);

static
globus_result_t
globus_l_gfs_blobseer_server_read(
    globus_l_gfs_blobseer_monitor_t *       monitor);


//FUNCTION IMPLEMENTATION

/* read data from the net */

static
void
globus_l_gfs_blobseer_recv_end_callback(
		globus_l_gfs_blobseer_monitor_t* monitor)
{
	GlobusGFSName(globus_l_gfs_blobseer_recv_end_callback);
	GlobusGFSBlobseerDebugEnter();

	/*when read the the end of file ;
	 number of read time from the net is equal to the number of write into the blobseer;
	 all the read data is written
	 and , we must wait all pending read is finish */
/*	if (monitor->eof)
	{
		printf("XXX\n Waiting ...\n");
		printf("XXXnumber read from net %d\n", monitor->number_reads_from_net);
		printf("XXX number write blob %d\n", monitor->number_write_blob);
		printf("XXX pending read %d\n", monitor->pending_reads);
	}*/
	if (monitor->eof && monitor->number_reads_from_net == monitor->number_write_blob && monitor->pending_reads ==0)
	  {
		  //register event to check the finish of tranfer
		  goto end_of_transfer;
	  }
	globus_callback_register_oneshot(GLOBUS_NULL,GLOBUS_NULL,globus_l_gfs_blobseer_recv_end_callback,monitor);
	GlobusGFSBlobseerDebugExit();
	return ;

end_of_transfer:
//	globus_mutex_unlock(&monitor->lock);
//	puts("End of transfer");
//	printf("number of read %d\n", monitor->number_reads_from_net);
    globus_l_gfs_blobseer_file_finalize(monitor);
	globus_gridftp_server_finished_transfer(monitor->op, GLOBUS_SUCCESS);
	GlobusGFSBlobseerDebugExit();
	return;
}

void pswap(void *ptrA, void *ptrB)
{
	int * ptrtemp;
	ptrtemp= ptrA;
	ptrA = ptrB;
	ptrB = ptrtemp;
	return;
}
static
void
globus_l_gfs_blobseer_server_read_cb(
		globus_gfs_operation_t op,
		globus_result_t result,
		globus_byte_t * buffer,
		globus_size_t 	nbytes,
		globus_off_t 	offset,
		globus_bool_t	eof,
		void * user_arg)
{
	globus_l_gfs_blobseer_monitor_t *  monitor;
	globus_l_gfs_blobseer_buffer_info_t * buf_info;

	GlobusGFSName(globus_l_gfs_blobseer_server_read_cb);
	GlobusGFSBlobseerDebugEnter();
	//  printf("BBB buffer address is %X\n",*buffer);

	monitor = (globus_l_gfs_blobseer_monitor_t *) user_arg;
	globus_mutex_lock(&monitor->lock);
	{
		monitor->pending_reads --;
	    if (eof)
	    {
	    	monitor->eof = GLOBUS_TRUE;
	   // 	printf("END OF FILE at %d\n",monitor->number_reads_from_net);
	    	//register recv_end_callback event ;
	    	if (!monitor->register_end)
	    	{
	    	globus_module_activate(GLOBUS_COMMON_MODULE);
	    	monitor->register_end = GLOBUS_TRUE;
	    	globus_callback_register_oneshot(
	    			GLOBUS_NULL,
	    			GLOBUS_NULL,
	    			globus_l_gfs_blobseer_recv_end_callback,
	    			monitor);
	    	globus_module_deactivate(GLOBUS_COMMON_MODULE);
	    	}
	    }

		if (result != GLOBUS_SUCCESS)
			{
				globus_gfs_log_message(GLOBUS_GFS_LOG_ERR,"Call back function");
				goto error_callback;
			}
		if(nbytes > 0)
		  {
			  buf_info = (globus_l_gfs_blobseer_buffer_info_t *)
						globus_malloc(sizeof (globus_l_gfs_blobseer_buffer_info_t));
			  //issue a new buffer request
			  buf_info->buffer = (globus_byte_t * ) globus_malloc(monitor->block_size * sizeof(globus_byte_t));
			  if (buf_info->buffer == NULL)
			  {
				  globus_gfs_log_message(GLOBUS_GFS_LOG_ERR,"Memory allocation error ");
				  goto error_alloc;
			  }
			 // memcpy(buf_info->buffer,buffer,monitor->block_size * sizeof(globus_byte_t));

			  //pswap(buffer,buf_info->buffer);
			//  globus_free(buffer);
			  //set direct pointer of new buffer to the previous buffer
			  buf_info->buffer = buffer;

			  buf_info->offset = offset;
			  buf_info->length = nbytes;

			  //add to queue
			  globus_priority_q_enqueue(&monitor->queue,buf_info,buf_info);
			  monitor->number_reads_from_net ++ ;

			  //update new offset
			  monitor->file_offset += buf_info->length;
		  }
		globus_l_gfs_blobseer_dispatch_write(monitor);
	}
	globus_mutex_unlock(&monitor->lock);
	return;
error_alloc:
error_callback:
	globus_mutex_unlock(&monitor->lock);
	GlobusGFSBlobseerDebugExitWithError();
	return;
}


//this is the callback function for writing data into blobseer
void
globus_l_gfs_blobseer_writeinto_storage_callback(
		globus_l_gfs_blobseer_monitor_t *           monitor)
{
	globus_l_gfs_blobseer_buffer_info_t* buf_info;
	globus_result_t result;

	//get information from the queue, if there is

	buf_info = (globus_l_gfs_blobseer_buffer_info_t *)
			globus_priority_q_dequeue(&monitor->queue);
	if (buf_info)
	{
		monitor->number_write_blob ++;
		//printf("AAAWrintg %d\n", monitor->number_write_blob);
		//blobseer write
		result = globus_l_gfs_blobseer_file_write(monitor,buf_info->offset,buf_info->length,buf_info->buffer);
		if (result != 1)
		{
			globus_gfs_log_message(GLOBUS_GFS_LOG_ERR," Error in writing into blobseer");
			return ;
		}
		//update write
		globus_gridftp_server_update_bytes_written(monitor->op,monitor->file_offset,buf_info->length);

		//free memory
		//globus_memory_push_node(&monitor->mem,buf_info->buffer);
		globus_free(buf_info->buffer);
	}
	globus_free(buf_info);
	//request a new callback function to read
	globus_callback_register_oneshot(GLOBUS_NULL,GLOBUS_NULL,globus_l_gfs_blobseer_writeinto_storage_callback,monitor);
}

static
globus_result_t
globus_l_gfs_blobseer_dispatch_write(
    globus_l_gfs_blobseer_monitor_t *           monitor)
{
	GlobusGFSName(globus_l_gfs_blobseer_dispath_write);
	GlobusGFSBlobseerDebugEnter();
	{
		if (monitor->first_write)
					{
						globus_module_activate(GLOBUS_COMMON_MODULE);
						monitor->first_write = GLOBUS_FALSE;
					//	puts("First write");
						globus_callback_register_oneshot(GLOBUS_NULL,GLOBUS_NULL,globus_l_gfs_blobseer_writeinto_storage_callback,monitor);
						globus_module_deactivate(GLOBUS_COMMON_MODULE);
					}
		//call new read from the net
		globus_l_gfs_blobseer_server_read(monitor);
	}
	GlobusGFSBlobseerDebugExit();
}

/*
 * This function has only duty to call a new server read by register a new read
 * It is called by  globus_l_gfs_blobseer_local_dispath_write();
 */
static
globus_result_t
globus_l_gfs_blobseer_server_read(
    globus_l_gfs_blobseer_monitor_t *           monitor)
{
    globus_result_t                     result;
    globus_byte_t *                     buffer;
    GlobusGFSName(globus_l_gfs_hpss_local_write_cb);
    GlobusGFSBlobseerDebugEnter();

    if(!monitor->eof)
    {
     //   buffer = (globus_byte_t *) globus_memory_pop_node(&monitor->mem);
    	buffer = (globus_byte_t *) globus_malloc(monitor->block_size * sizeof(globus_byte_t));
        globus_assert(buffer);
        result = globus_gridftp_server_register_read(
            monitor->op,
            buffer,
            monitor->block_size,
            globus_l_gfs_blobseer_server_read_cb,
            monitor);
        monitor->pending_reads ++;
	    if(result != GLOBUS_SUCCESS)
        {
	    	globus_gfs_log_message(GLOBUS_GFS_LOG_ERR,"Error in server register read");
            monitor->error = GlobusGFSErrorObjWrapFailed(
                "globus_gridftp_server_register_read", result);
            goto error_register;
        }
    }
    GlobusGFSBlobseerDebugExit();
    return GLOBUS_SUCCESS;

error_register:
   // globus_memory_push_node(&monitor->mem, buffer);
	globus_free(buffer);
	globus_l_gfs_blobseer_file_finalize(monitor);
	globus_gridftp_server_finished_transfer(monitor->op, GLOBUS_SUCCESS);
	globus_l_gfs_blobseer_monitor_destroy(monitor);
    GlobusGFSBlobseerDebugExitWithError();
    return result;
}

/*************************************************************************
 *  recv
 *  ----
 *  This interface function is called when the client requests that a
 *  file be transfered to the server.
 *
 *  To receive a file the following functions will be used in roughly
 *  the presented order.  They are doced in more detail with the
 *  gridftp server documentation.
 *
 *      globus_gridftp_server_begin_transfer();
 *      globus_gridftp_server_register_read();
 *      globus_gridftp_server_finished_transfer();
 *
 *  @vdinh
 *  The work flow is from
 *  	recv -> server_read_cb -> dispatch_write
 *                   ^                  |
 *                   |                  v
 *                   <-----------  server_read
 *
 ************************************************************************/

static
void
globus_l_gfs_blobseer_recv(
    globus_gfs_operation_t              op,
    globus_gfs_transfer_info_t *        transfer_info,
    void *                              user_arg)
{
    globus_l_gfs_blobseer_monitor_t *	monitor;
    globus_size_t						block_size;
    int 								optimal_count;
    globus_result_t						result;

    GlobusGFSName(globus_l_gfs_blobseer_recv);
    GlobusGFSBlobseerDebugEnter();

    globus_gfs_log_message(
        GLOBUS_GFS_LOG_INFO,
        "BLOBSEER STOR...\n");

    globus_gfs_log_message(
            GLOBUS_GFS_LOG_INFO,
            "BLOBSEER :Sending data from client to server...\n");

	globus_gridftp_server_get_optimal_concurrency(op, &optimal_count);//number of parallelism
	globus_gridftp_server_get_block_size(op, &block_size);//get block size
	globus_assert(optimal_count > 0 && block_size > 0); //for what ???

	printf("optimal count is %d\n", optimal_count);
	printf("block size is %d\n", block_size);

    //INIT monitor variable
    result = globus_l_gfs_blobseer_monitor_init(&monitor,block_size,optimal_count);
    if(result != GLOBUS_SUCCESS)
      {
		  globus_gfs_log_message(GLOBUS_GFS_LOG_ERR,"Error in initialize the blobseer");
          result = GlobusGFSErrorWrapFailed(
              "globus_l_gfs_blobseer_monitor_init", result);
          goto error_alloc;
      }

    monitor->op = op;

    /* CREATE FILE */
    globus_l_gfs_blobseer_file_init(monitor);
    result =  globus_l_gfs_blobseer_file_create(transfer_info->pathname,monitor);

    /*BEGIn TRANSFER */
    globus_gridftp_server_begin_transfer(monitor->op,0,monitor);
    globus_mutex_lock(&monitor->lock);//in case of parallelism
       {
    	while (monitor->optimal_count--)
    	{
    		//using memmory menagement structure
    		//globus_byte_t * buffer = (globus_byte_t *) globus_memory_pop_node(&monitor->mem);
    		//or : using globus mememory command :
    		globus_byte_t * buffer = (globus_byte_t *) globus_malloc(monitor->block_size * sizeof(globus_byte_t));
    		printf("optimal count is %d DDD init address is %X\n",monitor->optimal_count,*buffer);
    		result = globus_gridftp_server_register_read(monitor->op,buffer,monitor->block_size,globus_l_gfs_blobseer_server_read_cb,monitor);
       		if (result != GLOBUS_SUCCESS)
    		{
    			globus_gfs_log_message(GLOBUS_GFS_LOG_ERR,"globus_gridftp_server_register_read error");
    			goto error_register;
    		}
    		monitor->pending_reads ++;
		}
       }
    globus_mutex_unlock(&monitor->lock);
    GlobusGFSBlobseerDebugExit();
    return;
    //END OF TRANSFER DATA
error_alloc:
	globus_l_gfs_blobseer_monitor_destroy(monitor);
	GlobusGFSBlobseerDebugExitWithError();
	return;
error_register:
	globus_l_gfs_blobseer_file_finalize(monitor);
    globus_gridftp_server_finished_transfer(op,result);
	globus_l_gfs_blobseer_monitor_destroy(monitor);
    GlobusGFSBlobseerDebugExitWithError();
}
/*
 * ***************************************END FOR STOR ***************************
 */




/*
 * ****************************************FOR RETRIVING DATA *******************
 * Retriving data from Server to Client
 * Server : read data from blobseer
 * Transfer data
 * Client : write data to local file system.
 */

/* FUNCTION PROTOTYPE */

static
globus_result_t
globus_l_gfs_blobseer_dispatch_read(
		globus_l_gfs_blobseer_monitor_t* monitor);

static
void
globus_l_gfs_blobseer_server_write(
		globus_l_gfs_blobseer_monitor_t* monitor,
		globus_byte_t* buffer,
		globus_size_t offset,
		globus_size_t nbytes);

static
void
globus_l_gfs_blobseer_server_write_cb(
	globus_gfs_operation_t op,
	globus_result_t result,
	globus_byte_t * buffer,
	globus_size_t nbytes,
	void* user_arg);

//alway get data from blobseer to the queue
void
globus_l_gfs_blobseer_readfrom_storage_callback(
		globus_l_gfs_blobseer_monitor_t *           monitor);

/* FUNCTION IMPLEMENATION  */
static
globus_result_t
globus_l_gfs_blobseer_dispatch_read(
		globus_l_gfs_blobseer_monitor_t* monitor)
{
	globus_l_gfs_blobseer_buffer_info_t* buf_info;
	globus_byte_t * buffer;
	globus_size_t 	offset = 0;
	globus_size_t	nbytes = 0;

	GlobusGFSName(globus_l_gfs_blobseer_dispatch_read);
	GlobusGFSBlobseerDebugEnter();

	if (monitor->first_read)
	{
		monitor->read_offset = 0;
		monitor->first_read = GLOBUS_FALSE;
		globus_module_activate(GLOBUS_COMMON_MODULE);
		globus_callback_register_oneshot(GLOBUS_NULL,
				GLOBUS_NULL,
				globus_l_gfs_blobseer_readfrom_storage_callback,
				monitor);
		globus_module_deactivate(GLOBUS_COMMON_MODULE);
	}

	/*
	 * if the buffer list is not empty,
	 * get the data from the queue
	 * and push it to the buffer list
	 */
	while (!globus_list_empty(monitor->buffer_list)){
	if (!globus_list_empty(monitor->buffer_list))
	{
		buffer = globus_list_remove(&monitor->buffer_list,monitor->buffer_list);//get from the buffer list
		globus_assert(buffer);
	}
	else goto error_alloc;

	buf_info = (globus_l_gfs_blobseer_buffer_info_t *)
				globus_priority_q_dequeue(&monitor->queue);
	if (buf_info)
	{
		if (buf_info->buffer)
		{
			offset = buf_info->offset;
			nbytes = buf_info->length;
			memcpy(buffer,buf_info->buffer,0 * sizeof(globus_byte_t));
       	    free(buf_info->buffer);
		}
		//free the buf_info;
		free(buf_info);
	}

	//call a write into net
    globus_l_gfs_blobseer_server_write(monitor,buffer,offset,nbytes); //parameter lens is not important
	}
	GlobusGFSBlobseerDebugExit();
	return GLOBUS_SUCCESS;

error_alloc:
	globus_gfs_log_message(GLOBUS_GFS_LOG_ERR,"Error in allocation memory");
	GlobusGFSBlobseerDebugExitWithError();
	return  GLOBUS_ERROR_TYPE_ABORTED;

}

/*
 * vdinh
 * in server_write function, we will register the callback function for the event of write by using function
 * globus_gridftp_server_register_write()
 */
static
void
globus_l_gfs_blobseer_server_write(
		globus_l_gfs_blobseer_monitor_t* monitor,
		globus_byte_t* buffer,
		globus_size_t offset,
		globus_size_t nbytes)
{
	globus_result_t result;
	GlobusGFSName(globus_l_gfs_blobseer_server_write);
	GlobusGFSBlobseerDebugEnter();

	if (nbytes > 0)
	{
		result = globus_gridftp_server_register_write(monitor->op,
			buffer,
			nbytes,
			offset,
			-1, //stripe_ndx  - An integer identifying the stripe to add channels to. In the case of non-striped transfer this parameter will be ignored.
			globus_l_gfs_blobseer_server_write_cb, // this callback function is register for the event write
			monitor); //this value will be used as user_arg in the callback function
		if (result != GLOBUS_SUCCESS)
		{
			globus_gfs_log_message(GLOBUS_GFS_LOG_ERR,"globus_gridftp_server_register_write call backfunction error");
			goto error_callback;
		}
		monitor->pending_writes ++;
	}
	else
	{
		   globus_list_insert(&monitor->buffer_list, buffer);
		   //return to the dispath read
		   //in case of queue is empty but the the file is not reach to the end.
		   //due to speed for write to net is slower than the speed of read from blobseer storage
		   globus_l_gfs_blobseer_dispatch_read(monitor);
	}

	GlobusGFSBlobseerDebugExit();
	return 0;

error_callback:
	GlobusGFSBlobseerDebugExitWithError();
	return 0;
}

static
void
globus_l_gfs_blobseer_server_write_cb(
		globus_gfs_operation_t op,
		globus_result_t result,
		globus_byte_t * buffer,
		globus_size_t nbytes,
		void* user_arg)
{
	GlobusGFSName(globus_l_gfs_blobseer_server_write_cb);
	GlobusGFSBlobseerDebugEnter();
	if (result != GLOBUS_SUCCESS)
	    	{
				globus_gfs_log_message(GLOBUS_GFS_LOG_DUMP,"globus_l_gfs_blobseer_server_write_cb error");
	    		goto error_callback;
	    	}
    globus_l_gfs_blobseer_monitor_t * monitor;
    monitor = (globus_l_gfs_blobseer_monitor_t *) user_arg;

    globus_mutex_lock(&monitor->lock);
    {
	   globus_list_insert(&monitor->buffer_list, buffer);
	   monitor->number_writes_to_net ++;
	   monitor->pending_writes --;
	    /* if end of file */
       if (monitor->eof && monitor->number_read_blob == monitor->number_writes_to_net && monitor->pending_writes ==0)
	    {
	    //	puts("end of tranfer");
	    	goto end_of_transfer;
	    }

	///   printf("Writing to net %d\n", monitor->number_writes_to_net);
	   globus_l_gfs_blobseer_dispatch_read(monitor);

    }
   globus_mutex_unlock(&monitor->lock);
   GlobusGFSBlobseerDebugExit();
   return;
error_callback:
	globus_mutex_unlock(&monitor->lock);
	GlobusGFSDebugExitWithError();
	return;
end_of_transfer:
	globus_mutex_unlock(&monitor->lock);
	globus_l_gfs_blobseer_file_finalize(monitor);
	globus_gridftp_server_finished_transfer(monitor->op, GLOBUS_SUCCESS);
    globus_l_gfs_blobseer_monitor_destroy(monitor);
	GlobusGFSBlobseerDebugExit();
	return;

}

void
globus_l_gfs_blobseer_readfrom_storage_callback(
		globus_l_gfs_blobseer_monitor_t *           monitor)
{
	globus_l_gfs_blobseer_buffer_info_t* buf_info;
	globus_size_t read_length;
	GlobusGFSName(globus_l_gfs_blobseer_readfrom_storage_callback);
	GlobusGFSBlobseerDebugEnter();

	//globus_mutex_lock(&monitor->blob_lock);
	//allocate memeory
	buf_info = (globus_l_gfs_blobseer_buffer_info_t*)
			globus_malloc(sizeof(globus_l_gfs_blobseer_buffer_info_t));
	if (!buf_info)
	{
		goto error_alloc;
	}
	buf_info -> buffer = (globus_byte_t *) globus_malloc(monitor->block_size * sizeof(globus_byte_t));
	if (!buf_info->buffer)
	{
		goto error_alloc;
	}
	//read data from the blobseeer storage
	if (monitor->read_length > monitor->block_size)
	    	   	   read_length =  monitor->block_size;
	    	   else
	    	      	read_length = monitor->read_length;

   	monitor->number_read_blob ++ ;
globus_byte_t * buffer = (globus_byte_t *) globus_malloc(monitor->block_size*sizeof(globus_byte_t));
   	globus_l_gfs_blobseer_file_read(monitor,monitor->read_offset,read_length,buffer);
   	buf_info->length = read_length;
   	buf_info->offset = monitor->read_offset;

   	//add to queue
   	globus_priority_q_enqueue(&monitor->queue,buf_info,buf_info);

   	//update for new read
   	monitor->read_offset += read_length;
   	monitor->read_length -= read_length;
   	if (monitor->read_length <= 0)
   		monitor->eof = GLOBUS_TRUE;
   	if (!monitor->eof)
   	{
   		//call a new read
   		globus_callback_register_oneshot(GLOBUS_NULL,GLOBUS_NULL,globus_l_gfs_blobseer_readfrom_storage_callback,monitor);
   	}
   	else {
   		puts("END Of FILE ");
   	}
 //  	globus_mutex_unlock(&monitor->blob_lock);
   	GlobusGFSBlobseerDebugExit();
   	return ;
error_alloc:
	globus_gfs_log_message(GLOBUS_GFS_LOG_ERR,"Memory allocation error ");
	GlobusGFSBlobseerDebugExitWithError();
	return ;
}
/*************************************************************************
 *  send
 *  ----
 *  This interface function is called when the client requests to receive
 *  a file from the server.
 *
 *  To send a file to the client the following functions will be used in roughly
 *  the presented order.  They are doced in more detail with the
 *  gridftp server documentation.
 *
 *      globus_gridftp_server_begin_transfer();
 *      globus_gridftp_server_register_write();
 *      globus_gridftp_server_finished_transfer();
 *
 *  Here are the work flow for the RETR
 *
 *  send ---> dispatch read --> server write
 *  			   ^                 |
 *  			   |                 V
 *  			   <------------server_write_cb
 *
 ************************************************************************/
/*
 * vdinh : transfer_info->pathname will stored the name of the sending file
 */
static
void
globus_l_gfs_blobseer_send(
    globus_gfs_operation_t              op,
    globus_gfs_transfer_info_t *        transfer_info,
    void *                              user_arg)
{
    globus_l_gfs_blobseer_handle_t *       blobseer_handle;
    globus_l_gfs_blobseer_monitor_t *		monitor;
    globus_size_t							block_size;
    int										optimal_count;
    globus_result_t							result;

    GlobusGFSName(globus_l_gfs_blobseer_send);
    GlobusGFSBlobseerDebugEnter();
    globus_gfs_log_message(
        GLOBUS_GFS_LOG_INFO,
        "Blobseer : Retrieving data from Server to Client ...\n");
    blobseer_handle = (globus_l_gfs_blobseer_handle_t *) user_arg;

    globus_gridftp_server_get_optimal_concurrency(op,&optimal_count);
    globus_gridftp_server_get_block_size(op,&block_size);
    printf("optimal count is %d\n",optimal_count);
    printf("block size is %d\n", block_size);
    globus_assert(optimal_count > 0 && block_size > 0);

    /* init the monitor resource */
    result = globus_l_gfs_blobseer_monitor_init(&monitor,block_size,optimal_count);
    /* init the buffer cached */
    while (optimal_count--)
     {
     	globus_byte_t * buffer;
     	buffer = globus_memory_pop_node(&monitor->mem); //memory allocation for buffer variable
     	globus_list_insert(&monitor->buffer_list, buffer);
     }
    monitor->op = op;
    monitor->pathname = globus_libc_strdup(transfer_info->pathname);

    /* OPEN A FILE FROM A SERVER */
    globus_l_gfs_blobseer_file_init(monitor);
    result = globus_l_gfs_blobseer_file_open(monitor->pathname,monitor);
    if (!result)
    {
    	globus_gfs_log_message(GLOBUS_GFS_LOG_ERR,"File open error");
    	goto error_file_open;
    }

    /*BEGIN TRANSFER */
    globus_gridftp_server_begin_transfer(monitor->op,0, monitor);

    globus_mutex_lock(&monitor->lock);
    {
    	result = globus_l_gfs_blobseer_dispatch_read(monitor);
    	if (result != GLOBUS_SUCCESS)
    	{
    		globus_gfs_log_message(GLOBUS_GFS_LOG_ERR," Globus blobseer dispatch error");
    		goto error_dispatch;
    	}
    };
    globus_mutex_unlock(&monitor->lock);
    GlobusGFSBlobseerDebugExit();
    return;
error_dispatch :
	globus_mutex_unlock(&monitor->lock);
error_file_open:
	globus_l_gfs_blobseer_file_finalize(monitor);
	globus_gridftp_server_finished_transfer(monitor->op,GLOBUS_FAILURE);
	globus_l_gfs_blobseer_monitor_destroy(monitor);
	GlobusGFSBlobseerDebugExitWithError();
	return;
}

/*
 * *********************END OF RETR ******************************
 */
static
int
globus_l_gfs_blobseer_activate(void);

static
int
globus_l_gfs_blobseer_deactivate(void);

/*
 *  no need to change this
 */
static globus_gfs_storage_iface_t       globus_l_gfs_blobseer_dsi_iface = 
{
    GLOBUS_GFS_DSI_DESCRIPTOR_BLOCKING | GLOBUS_GFS_DSI_DESCRIPTOR_SENDER,
    globus_l_gfs_blobseer_start,
    globus_l_gfs_blobseer_destroy,
    NULL, /* list */
    globus_l_gfs_blobseer_send,
    globus_l_gfs_blobseer_recv,
    NULL, /* trev */
    NULL, /* active */
    NULL, /* passive */
    NULL, /* data destroy */
    globus_l_gfs_blobseer_command, 
    globus_l_gfs_blobseer_stat,
    NULL,
    NULL
};

/*
 *  no need to change this
 */
GlobusExtensionDefineModule(globus_gridftp_server_blobseer) =
{
    "globus_gridftp_server_blobseer",
    globus_l_gfs_blobseer_activate,
    globus_l_gfs_blobseer_deactivate,
    NULL,
    NULL,
    &local_version
};

/*
 *  no need to change this
 */
static
int
globus_l_gfs_blobseer_activate(void)
{
    globus_gfs_log_message(
        GLOBUS_GFS_LOG_INFO,
        "Blobseer actived...\n");
    globus_extension_registry_add(
        GLOBUS_GFS_DSI_REGISTRY,
        "blobseer",
        GlobusExtensionMyModule(globus_gridftp_server_blobseer),
        &globus_l_gfs_blobseer_dsi_iface);
    
    return 0;
}
/*
 *  no need to change this
 */
static
int
globus_l_gfs_blobseer_deactivate(void)
{
    globus_gfs_log_message(
        GLOBUS_GFS_LOG_INFO,
        "Blobseer deactived...\n");
    globus_extension_registry_remove(
        GLOBUS_GFS_DSI_REGISTRY, "blobseer");

    return 0;
}

