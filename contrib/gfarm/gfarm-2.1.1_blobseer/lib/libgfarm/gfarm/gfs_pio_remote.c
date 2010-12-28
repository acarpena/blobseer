/*
 * pio operations for remote fragment
 *
 * $Id: gfs_pio_remote.c 3811 2007-09-06 09:18:41Z tatebe $
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <sys/socket.h> /* struct sockaddr */
#include <openssl/evp.h>
#include <gfarm/gfarm.h>
#include "host.h"
#include "config.h"
#include "gfs_proto.h"	/* GFS_PROTO_FSYNC_* */
#include "gfs_client.h"
#include "gfs_io.h"
#include "gfs_pio.h"

static gfarm_error_t
gfs_pio_remote_storage_close(GFS_File gf)
{
	gfarm_error_t e;
	struct gfs_file_section_context *vc = gf->view_context;
	struct gfs_connection *gfs_server = vc->storage_context;

	/*
	 * Do not close remote file from a child process because its
	 * open file count is not incremented.
	 * XXX - This behavior is not the same as expected, but better
	 * than closing the remote file.
	 */
	if (vc->pid != getpid())
		return (GFARM_ERR_NO_ERROR);
	e = gfs_client_close(gfs_server, gf->fd);
	gfs_client_connection_free(gfs_server);
	return (e);
}

static gfarm_error_t
gfs_pio_remote_storage_pwrite(GFS_File gf,
	const char *buffer, size_t size, gfarm_off_t offset, size_t *lengthp)
{
	struct gfs_file_section_context *vc = gf->view_context;
	struct gfs_connection *gfs_server = vc->storage_context;

	/*
	 * buffer beyond GFS_PROTO_MAX_IOSIZE are just ignored by gfsd,
	 * we don't perform such GFS_PROTO_WRITE request, because it's
	 * inefficient.
	 * Note that upper gfs_pio layer should care this partial write.
	 */
	if (size > GFS_PROTO_MAX_IOSIZE)
		size = GFS_PROTO_MAX_IOSIZE;
	return (gfs_client_pwrite(gfs_server, gf->fd, buffer, size, offset,
	    lengthp));
}

static gfarm_error_t
gfs_pio_remote_storage_pread(GFS_File gf,
	char *buffer, size_t size, gfarm_off_t offset, size_t *lengthp)
{
	struct gfs_file_section_context *vc = gf->view_context;
	struct gfs_connection *gfs_server = vc->storage_context;

	/*
	 * Unlike gfs_pio_remote_storage_write(), we don't care
	 * buffer size here, because automatic i/o size truncation
	 * performed by gfsd isn't inefficient for read case.
	 * Note that upper gfs_pio layer should care the partial read.
	 */
	return (gfs_client_pread(gfs_server, gf->fd, buffer, size, offset,
	    lengthp));
}

static gfarm_error_t
gfs_pio_remote_storage_ftruncate(GFS_File gf, gfarm_off_t length)
{
	struct gfs_file_section_context *vc = gf->view_context;
	struct gfs_connection *gfs_server = vc->storage_context;

	return (gfs_client_ftruncate(gfs_server, gf->fd, length));
}

static gfarm_error_t
gfs_pio_remote_storage_fsync(GFS_File gf, int operation)
{
	struct gfs_file_section_context *vc = gf->view_context;
	struct gfs_connection *gfs_server = vc->storage_context;

	return (gfs_client_fsync(gfs_server, gf->fd, operation));
}

static gfarm_error_t
gfs_pio_remote_storage_fstat(GFS_File gf, struct gfs_stat *st)
{
	struct gfs_file_section_context *vc = gf->view_context;
	struct gfs_connection *gfs_server = vc->storage_context;

	return (gfs_client_fstat(gfs_server, gf->fd,
	    &st->st_size, &st->st_atimespec.tv_sec, &st->st_atimespec.tv_nsec,
	    &st->st_mtimespec.tv_sec, &st->st_mtimespec.tv_nsec));
}

static int
gfs_pio_remote_storage_fd(GFS_File gf)
{
	/*
	 * Unlike Gfarm version 1, we tell the caller that
	 * gfs_client_connection_fd() isn't actually usable.
	 */
	return (-1);
}

//TRAN begin
static gfarm_error_t
gfs_pio_remote_storage_fget_version_count(GFS_File gf, size_t *nversion)
{
  //*nversion = 10;
	//return (GFARM_ERR_NO_ERROR);
       	struct gfs_file_section_context *vc = gf->view_context;
	struct gfs_connection *gfs_server = vc->storage_context;

	return (gfs_client_fget_version_count(gfs_server, gf->fd, nversion));

}

static gfarm_error_t
gfs_pio_remote_storage_fset_version(GFS_File gf, size_t nversion, size_t * nactive_version)
{
  //	nversion = 10;
  //return (GFARM_ERR_NO_ERROR);
       	struct gfs_file_section_context *vc = gf->view_context;
	struct gfs_connection *gfs_server = vc->storage_context;
        gf->active_version = nversion;
	return (gfs_client_fset_version(gfs_server, gf->fd, nversion , nactive_version));

}

static gfarm_error_t
gfs_pio_remote_storage_fget_current_version(GFS_File gf, size_t *nversion)
{
	*nversion = gf->active_version;
	return (GFARM_ERR_NO_ERROR);
}

static gfarm_error_t
gfs_pio_remote_storage_vread(size_t nversion, GFS_File gf,
	char *buffer, size_t size, gfarm_off_t offset, size_t *lengthp)
{
  //	nversion = 10;
  //return (GFARM_ERR_NO_ERROR);
  //       	struct gfs_file_section_context *vc = gf->view_context;
  //	struct gfs_connection *gfs_server = vc->storage_context;

  //	return (gfs_client_vread(nversion,gfs_server, gf->fd, buffer, size, offset,
  //	    lengthp));
  return (GFARM_ERR_NO_ERROR);
}

//TRAN end


struct gfs_storage_ops gfs_pio_remote_storage_ops = {
	gfs_pio_remote_storage_close,
	gfs_pio_remote_storage_fd,
	gfs_pio_remote_storage_pread,
	gfs_pio_remote_storage_pwrite,
	gfs_pio_remote_storage_ftruncate,
	gfs_pio_remote_storage_fsync,
	gfs_pio_remote_storage_fstat,
      	//TRAN begin
	gfs_pio_remote_storage_fget_version_count,
        gfs_pio_remote_storage_fset_version,
        gfs_pio_remote_storage_fget_current_version,
        gfs_pio_remote_storage_vread,
	//TRAN end

};

gfarm_error_t
gfs_pio_open_remote_section(GFS_File gf, struct gfs_connection *gfs_server)
{
	gfarm_error_t e;
	struct gfs_file_section_context *vc = gf->view_context;

	e = gfs_client_open(gfs_server, gf->fd);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	vc->ops = &gfs_pio_remote_storage_ops;
	vc->storage_context = gfs_server;
	vc->fd = -1; /* not used */
	vc->pid = getpid();
	return (GFARM_ERR_NO_ERROR);
}
