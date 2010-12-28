/*
 * pio operations for local section/BlobSeer direct access mode
 *
 * $Id: gfs_pio_local.c 3811 2007-09-06 09:18:41Z tatebe $
 */
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h> /* gfs_client.h needs socklen_t */
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>
#include <openssl/evp.h>
#include <gfarm/gfarm.h>

#include "gfs_proto.h"	/* GFS_PROTO_FSYNC_* */
#include "gfs_client.h"
#include "gfs_io.h"
#include "gfs_pio.h"
#include "config.h"	 //TRAN it was not here. 

#if 0 /* not yet in gfarm v2 */

int gfarm_node = -1;
int gfarm_nnode = -1;

gfarm_error_t
gfs_pio_set_local(int node, int nnode)
{
	if (node < 0 || node >= nnode || nnode < 0)
		return (GFARM_ERR_INVALID_ARGUMENT);

	gfarm_node = node;
	gfarm_nnode = nnode;
	return (GFARM_ERR_NO_ERROR);
}

gfarm_error_t
gfs_pio_set_local_check(void)
{
	if (gfarm_node < 0 || gfarm_nnode <= 0)
		return ("gfs_pio_set_local() is not correctly called");
	return (GFARM_ERR_NO_ERROR);
}

gfarm_error_t
gfs_pio_get_node_rank(int *node)
{
	gfarm_error_t e = gfs_pio_set_local_check();

	if (e != GFARM_ERR_NO_ERROR)
		return (e);
	*node = gfarm_node;
	return (GFARM_ERR_NO_ERROR);
}

gfarm_error_t
gfs_pio_get_node_size(int *nnode)
{
	gfarm_error_t e = gfs_pio_set_local_check();

	if (e != GFARM_ERR_NO_ERROR)
		return (e);
	*nnode = gfarm_nnode;
	return (GFARM_ERR_NO_ERROR);
}

gfarm_error_t
gfs_pio_set_view_local(GFS_File gf, int flags)
{
	gfarm_error_t e;
	char *arch;

	if (GFS_FILE_IS_PROGRAM(gf)) {
		e = gfarm_host_get_self_architecture(&arch);
		if (e != GFARM_ERR_NO_ERROR)
			return (gf->error = e);
		return (gfs_pio_set_view_section(gf, arch, NULL, flags));
	}
	e = gfs_pio_set_local_check();
	if (e != GFARM_ERR_NO_ERROR)
		return (gf->error = e);
	return (gfs_pio_set_view_index(gf, gfarm_nnode, gfarm_node,
				       NULL, flags));
}

#endif /* not yet in gfarm v2 */

/***********************************************************************/
//TRAN begin
static blob_env_t env;	//handling the env in order to access BlobSeer
//TRAN end

static gfarm_error_t
gfs_pio_local_storage_close(GFS_File gf)
{
	gfarm_error_t e, e2;
	struct gfs_file_section_context *vc = gf->view_context;
	struct gfs_connection *gfs_server = vc->storage_context;

	//free the blob handle
	blob_free(&env, &(vc->blob));
	e = GFARM_ERR_NO_ERROR;

	/*
	 * Do not close remote file from a child process because its
	 * open file count is not incremented.
	 * XXX - This behavior is not the same as expected, but better
	 * than closing the remote file.
	 */
	if (vc->pid != getpid())
		return (e);
	e2 = gfs_client_close(gfs_server, gf->fd);
	gfs_client_connection_free(gfs_server);
	return (e != GFARM_ERR_NO_ERROR ? e : e2);
}

static gfarm_error_t
gfs_pio_local_storage_pwrite(GFS_File gf,
	const char *buffer, size_t size, gfarm_off_t offset, size_t *lengthp)
{
	struct gfs_file_section_context *vc = gf->view_context;

		int rv ;
		rv = 0;
		gfarm_off_t blob_size; //we may change the type to gfarm_int64_t
		blob_size = 0 ;
		blob_t *blob = &(vc->blob);
                
                blob_getlatest(blob);
                blob_size = blob_getsize(blob,blob->latest_version);	//get the blob size of an specified version
                
		if(blob_size > size + offset)
		{
			gfarm_off_t new_size;
			gfarm_off_t new_offset;
						
			new_size = size ;
			new_offset = offset;
							
			gfarm_off_t rdoff, rdsize;
			rdoff = offset%BLOB_PAGE_SIZE;
			rdsize = size % BLOB_PAGE_SIZE;
			if( rdoff !=0 || rdsize != 0)
			{
				new_offset = offset-rdoff;
				new_size = ((size + rdoff -1)/BLOB_PAGE_SIZE +1 ) * BLOB_PAGE_SIZE ;
				
				char * blob_buffer;
				blob_buffer = malloc(new_size);	//temporary buffer
				memcpy(blob_buffer+rdoff,buffer,size);
				rv = blob_write(blob,new_offset,new_size,blob_buffer);
				free(blob_buffer);
			}
			else
			{
				rv = blob_write(blob,offset,size,buffer);
			}
			
			if (rv !=0) rv= size;
			
		}
		else if (blob_size >= offset)
		{
			gfarm_off_t new_size;
			gfarm_off_t new_offset;
			gfarm_off_t rdoff;

			rdoff = offset%BLOB_PAGE_SIZE;

			new_offset = offset-rdoff;
			new_size = ((size + rdoff -1)/BLOB_PAGE_SIZE +1 ) * BLOB_PAGE_SIZE ;

			char * blob_buffer;

			blob_buffer = malloc(new_size);	//temporary buffer
			memcpy(blob_buffer+rdoff,buffer,size);

			rv = blob_write(blob,new_offset,new_size,blob_buffer);
			if (rv !=0 ) rv = size;
					
			free(blob_buffer);
		}
		else rv = 0;

	if (rv == -1)
		return (gfarm_errno_to_error(errno));
	*lengthp = rv;
	return (GFARM_ERR_NO_ERROR);
}

static gfarm_error_t
gfs_pio_local_storage_pread(GFS_File gf,
	char *buffer, size_t size, gfarm_off_t offset, size_t *lengthp)
{
	struct gfs_file_section_context *vc = gf->view_context;
	int rv ;
	gfarm_off_t blob_size;

	blob_t *blob = &(vc->blob);
        if ( gf->active_version == 0 )
          {
            blob_getlatest(blob);
            blob_size = blob_getsize(blob,blob->latest_version);
          }
        else
          {
            blob_size = blob_getsize(blob,gf->active_version);	//get the blob size
          }

	if(blob_size > size + offset)
	{
		gfarm_off_t new_size;
		gfarm_off_t new_offset;
					
		new_size = size ;
		new_offset = offset;
						
		gfarm_off_t rdoff;
		rdoff = offset%BLOB_PAGE_SIZE;

		new_offset = offset-rdoff;
		new_size = ((size + rdoff -1)/BLOB_PAGE_SIZE +1 ) * BLOB_PAGE_SIZE ;
		char * blob_buffer;
		blob_buffer = malloc(new_size);	//temporary buffer
                if ( gf->active_version == 0 )
                  rv = blob_read(blob,blob->latest_version,new_offset,new_size,blob_buffer);
                else
                  rv = blob_read(blob,gf->active_version,new_offset,new_size,blob_buffer);
		if (rv !=0 ) memcpy(buffer, blob_buffer+rdoff,size);
		else rv = -1;
		rv= size;
		free(blob_buffer);
	}
	else if (blob_size > offset)
	{
		gfarm_off_t new_size;
		gfarm_off_t new_offset;
							
		gfarm_off_t rdoff;
		rdoff = offset%BLOB_PAGE_SIZE;

		new_offset = offset-rdoff;
		new_size = blob_size - new_offset;

		char * blob_buffer;

		blob_buffer = malloc(new_size);	//temporary buffer

		if (new_size !=0 )
                  {
                    if ( gf->active_version == 0 )
                      rv = blob_read(blob,blob->latest_version,new_offset,new_size,blob_buffer);
                    else
                      rv = blob_read(blob,gf->active_version,new_offset,new_size,blob_buffer);
                  }
		rv = blob_size - offset;
		memcpy(buffer, blob_buffer+rdoff,rv);
		
		free(blob_buffer);
	}
	else rv = 0;
	if (rv == -1)
		return (gfarm_errno_to_error(errno));
	*lengthp = rv;
	return (GFARM_ERR_NO_ERROR);
}



static gfarm_error_t
gfs_pio_local_storage_ftruncate(GFS_File gf, gfarm_off_t length)
{

//Not available, the function should return NO_ERROR

//	struct gfs_file_section_context *vc = gf->view_context;
//TRAN begin
//	int rv;

//	rv = ftruncate(vc->fd, length);
//	if (rv == -1)
//		return (gfarm_errno_to_error(errno));
//TRAN end	
	return (GFARM_ERR_NO_ERROR);
}

static gfarm_error_t
gfs_pio_local_storage_fsync(GFS_File gf, int operation)
{
//Not available, the function should return NO_ERROR

//	struct gfs_file_section_context *vc = gf->view_context;
//	int rv;
//TRAN begin
/*	switch (operation) {
	case GFS_PROTO_FSYNC_WITHOUT_METADATA:
#ifdef HAVE_FDATASYNC
		rv = fdatasync(vc->fd);
		break;
#else
*/		/*FALLTHROUGH*/
/*
#endif
	case GFS_PROTO_FSYNC_WITH_METADATA:
		rv = fsync(vc->fd);
		break;
	default:
		return (GFARM_ERR_INVALID_ARGUMENT);
	}

	if (rv == -1)
		return (gfarm_errno_to_error(errno));
*/	return (GFARM_ERR_NO_ERROR);
}

static gfarm_error_t
gfs_pio_local_storage_fstat(GFS_File gf, struct gfs_stat *st)
{

//Not available, the function should return NO_ERROR

//	struct gfs_file_section_context *vc = gf->view_context;
//	struct stat sb;

//	if (fstat(vc->fd, &sb) == 0) {
//		st->st_size = sb.st_size;
//		st->st_atimespec.tv_sec = sb.st_atime;
//		st->st_atimespec.tv_nsec = 0; /* XXX */
//		st->st_mtimespec.tv_sec = sb.st_mtime;
//		st->st_mtimespec.tv_nsec = 0; /* XXX */
//	}
//	else
//		return (gfarm_errno_to_error(errno));

	return (GFARM_ERR_NO_ERROR);
}

static int
gfs_pio_local_storage_fd(GFS_File gf)
{
	struct gfs_file_section_context *vc = gf->view_context;

	return (vc->fd);
}
//TRAN begin
//this function returns the latest blob version

static gfarm_error_t
gfs_pio_local_storage_fget_version_count(GFS_File gf, size_t *nversion)
{
	struct gfs_file_section_context *vc = gf->view_context;
        blob_t  *blob =&(vc->blob);

        if (blob_getlatest(blob) == 1)
          gf->latest_version = blob->latest_version;
        else gf->latest_version = -1;
	*nversion = gf->latest_version;
	return (GFARM_ERR_NO_ERROR);
}

//this function is for switching to the other version of the opened blob
static gfarm_error_t
gfs_pio_local_storage_fset_version(GFS_File gf, size_t nversion,size_t * nactive_version)
{
	struct gfs_file_section_context *vc = gf->view_context;
        blob_t  *blob =&(vc->blob);

    if (blob_getlatest(blob))
    	{
			gf->latest_version = blob->latest_version;

    	    if (nversion <= -1 || nversion > gf->latest_version)
    	    {
    	    	gf->active_version = gf->latest_version;
    	     }
    	    else
    	    	gf->active_version = nversion;

    	    *nactive_version = gf->active_version;
      	}
    return (GFARM_ERR_NO_ERROR);
}

//getting back the active version of the opened blob
static gfarm_error_t
gfs_pio_local_storage_fget_current_version(GFS_File gf, size_t *nversion)
{
        //gf->active_version = 10;
	*nversion = gf->active_version;
	return (GFARM_ERR_NO_ERROR);
}


//not yet implemented
static gfarm_error_t
gfs_pio_local_storage_vread(size_t nversion, GFS_File gf,
	char *buffer, size_t size, gfarm_off_t offset, size_t *lengthp)
{
	nversion = 10;
	return (GFARM_ERR_NO_ERROR);
}

//TRAN end
struct gfs_storage_ops gfs_pio_local_storage_ops = {
	gfs_pio_local_storage_close,
	gfs_pio_local_storage_fd,
	gfs_pio_local_storage_pread,
	gfs_pio_local_storage_pwrite,
	gfs_pio_local_storage_ftruncate,
	gfs_pio_local_storage_fsync,
	gfs_pio_local_storage_fstat,
	//TRAN begin
	//versioning interface of BlobSeer direct access mode
	gfs_pio_local_storage_fget_version_count,
        gfs_pio_local_storage_fset_version,
        gfs_pio_local_storage_fget_current_version,
        gfs_pio_local_storage_vread,
	//TRAN end
};

//This test function is used for testing purpose
void test_blobseer_lib(void)
{
    	blob_t blob;
	if (!blob_create(&env, BLOB_PAGE_SIZE, 1, &blob)) {
	        printf("Error, cannot alloc!\n");
	        return;
	    }
	blob_free(&env, &blob);
}

//Initializing the BlobSeer direct access mode. 
//Basically, it was Gfarm local access mode. We reused this for keeping the modification simple.

gfarm_error_t
gfs_pio_open_local_section(GFS_File gf, struct gfs_connection *gfs_server)
{
	struct gfs_file_section_context *vc = gf->view_context;
	gfarm_error_t e;

	//initializing the BlobSeer direct access mode. We will change the function name later	
	e = gfs_client_open_local(gfs_server, gf->fd, &vc->fd);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	vc->ops = &gfs_pio_local_storage_ops;
	vc->storage_context = gfs_server;
	vc->pid = getpid();

	char * strconfig = BLOB_CONFIGURATION;
	blob_init(strconfig, &env);
	
	//verifying if it works well with BlobSeer
	//test_blobseer_lib();
	
	//open the blob after getting the blob_id from the gfsd daemon
	if (!blob_setid(&env, vc->fd, &(vc->blob))) {
				return gfarm_errno_to_error(-1);  // set errno =  -1,
				}
	//gfarm_metadb_server_port = 12;
	return (GFARM_ERR_NO_ERROR);
}

