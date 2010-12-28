/*
 * $Id: gfs_pio.c 3839 2007-10-05 06:41:45Z tatebe $
 */

#include <sys/types.h> /* mode_t */
#include <sys/stat.h> /* umask() */
#include <sys/time.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>	/* [FRWX]_OK */
#include <errno.h>
#include <openssl/evp.h>

#define GFARM_INTERNAL_USE
#include <gfarm/gfarm.h>

#include "timer.h"
#include "gfutil.h"

#include "liberror.h"
#include "gfs_profile.h"
#include "gfs_proto.h"	/* GFS_PROTO_FSYNC_* */
#include "gfs_io.h"
#include "gfs_pio.h"

#include "config.h" /* XXX FIXME this shouldn't be needed here */

/*
 * GFARM_ERRMSG_GFS_PIO_IS_EOF is used as mark of EOF,
 * and shouldn't be returned to caller functions.
 */

int
gfs_pio_eof(GFS_File gf)
{
	return (gf->error == GFARM_ERRMSG_GFS_PIO_IS_EOF);
}

#define GFS_PIO_ERROR(gf) \
	((gf)->error != GFARM_ERRMSG_GFS_PIO_IS_EOF ? \
	 (gf)->error : GFARM_ERR_NO_ERROR)

gfarm_error_t
gfs_pio_error(GFS_File gf)
{
	return (GFS_PIO_ERROR(gf));
}

void
gfs_pio_clearerr(GFS_File gf)
{
	gf->error = GFARM_ERR_NO_ERROR;
}

static gfarm_error_t
gfs_pio_is_view_set(GFS_File gf)
{
	return (gf->view_context != NULL);
}

gfarm_error_t
gfs_pio_set_view_default(GFS_File gf)
{
	gfarm_error_t e, e_save = GFARM_ERR_NO_ERROR;

	if (gfs_pio_is_view_set(gf)) {
		if ((gf->mode & GFS_FILE_MODE_WRITE) != 0)
			e_save = gfs_pio_flush(gf);
		e = (*gf->ops->view_close)(gf);
		if (e_save == GFARM_ERR_NO_ERROR)
			e_save = e;
	}
	gf->ops = NULL;
	gf->view_context = NULL;
#if 0 /* not yet in gfarm v2 */
	gf->view_flags = 0;
#endif /* not yet in gfarm v2 */
	gf->error = e_save;
	return (e_save);
}

static gfarm_error_t
gfs_pio_check_view_default(GFS_File gf)
{
	gfarm_error_t e;

	e = GFS_PIO_ERROR(gf);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	if (!gfs_pio_is_view_set(gf)) /* view isn't set yet */
#if 0 /* not yet in gfarm v2 */
		return (gfs_pio_set_view_global(gf, 0));
#else /* not yet in gfarm v2 */
		return (gfs_pio_internal_set_view_section(gf, NULL, 0));
#endif /* not yet in gfarm v2 */
	return (GFARM_ERR_NO_ERROR);
}

/* gfs_pio_fileno returns a network-wide file descriptor in Gfarm v2 */
int gfs_pio_fileno(GFS_File gf)
{
	return (gf == NULL ? -1 : gf->fd);
}

static gfarm_error_t
gfs_file_close(struct gfs_desc *gd)
{
	return (gfs_pio_close((GFS_File)gd));
}

static gfarm_error_t
gfs_file_desc_to_file(struct gfs_desc *gd, struct gfs_file **gfp)
{
	*gfp = (GFS_File)gd;
	return (GFARM_ERR_NO_ERROR);
}

static gfarm_error_t
gfs_file_desc_to_dir(struct gfs_desc *gd, struct gfs_dir **gdp)
{
	return (GFARM_ERR_NOT_A_DIRECTORY);
}

gfarm_error_t
gfs_file_alloc(gfarm_int32_t fd, int flags, struct gfs_desc **gdp)
{
	static struct gfs_desc_ops gfs_file_desc_ops = {
		gfs_file_close,
		gfs_file_desc_to_file,
		gfs_file_desc_to_dir,
	};
	GFS_File gf;
	char *buffer;

	GFARM_MALLOC(gf);
	GFARM_MALLOC_ARRAY(buffer, GFS_FILE_BUFSIZE);
	if (buffer == NULL || gf == NULL) {
		if (buffer != NULL)
			free(buffer);
		if (gf != NULL)
			free(gf);
		return (GFARM_ERR_NO_MEMORY);
	}
	memset(gf, 0, sizeof(*gf));
	gf->desc.ops = &gfs_file_desc_ops;

	gf->fd = fd;
	gf->mode = 0;
	switch (flags & GFARM_FILE_ACCMODE) {
	case GFARM_FILE_RDONLY:
		gf->mode |= GFS_FILE_MODE_READ;
		break;
	case GFARM_FILE_WRONLY:
		gf->mode |= GFS_FILE_MODE_WRITE;
		break;
	case GFARM_FILE_RDWR:
		gf->mode |= GFS_FILE_MODE_READ|GFS_FILE_MODE_WRITE;
		break;
	}

	gf->open_flags = flags;
	gf->error = GFARM_ERR_NO_ERROR;
	gf->io_offset = 0;

	gf->buffer = buffer;
	gf->p = 0;
	gf->length = 0;
	gf->offset = 0;
        //TRAN begin
        // reset active_version to zero
        gf->active_version = 0;
        gf->latest_version = 0;
        //TRAN end

	gf->view_context = NULL;
	gfs_pio_set_view_default(gf);

	*gdp = &gf->desc;
	return (GFARM_ERR_NO_ERROR);
}

static void
gfs_file_free(GFS_File gf)
{
	free(gf->buffer);
	/* do not touch gf->pi here */
	free(gf);
}

static double gfs_pio_create_time;
static double gfs_pio_open_time;
static double gfs_pio_close_time;
static double gfs_pio_seek_time;
static double gfs_pio_truncate_time;
static double gfs_pio_read_time;
static double gfs_pio_write_time;
static double gfs_pio_sync_time;
static double gfs_pio_datasync_time;
static double gfs_pio_getline_time;
static double gfs_pio_getc_time;
static double gfs_pio_putc_time;

gfarm_error_t
gfs_pio_create(const char *url, int flags, gfarm_mode_t mode, GFS_File *gfp)
{
	gfarm_error_t e;
	struct gfs_desc *gd;
	gfarm_timerval_t t1, t2;

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

	if ((e = gfs_desc_create(url, flags, mode, &gd)) != GFARM_ERR_NO_ERROR)
		;
	else if ((e = (*gd->ops->desc_to_file)(gd, gfp)) != GFARM_ERR_NO_ERROR)
		gfs_desc_close(gd);

	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(gfs_pio_create_time += gfarm_timerval_sub(&t2, &t1));
	return (e);
}

gfarm_error_t
gfs_pio_open(const char *url, int flags, GFS_File *gfp)
{
	gfarm_error_t e;
	struct gfs_desc *gd;
	gfarm_timerval_t t1, t2;

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));
	if ((e = gfs_desc_open(url, flags, &gd)) != GFARM_ERR_NO_ERROR)
		;
	else if ((e = (*gd->ops->desc_to_file)(gd, gfp)) != GFARM_ERR_NO_ERROR)
		gfs_desc_close(gd);

	//TRAN set the latest version as the default version
	size_t nversion = 0;
	gfs_set_version(*gfp,-1, &nversion);


	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(gfs_pio_open_time += gfarm_timerval_sub(&t2, &t1));
	return (e);
}

#if 0 /* not yet in gfarm v2 */

gfarm_error_t
gfs_pio_get_nfragment(GFS_File gf, int *nfragmentsp)
{
	if (GFS_FILE_IS_PROGRAM(gf))
		return (GFARM_ERR_OPERATION_NOT_PERMITTED);
	if ((gf->mode & GFS_FILE_MODE_NSEGMENTS_FIXED) == 0)
		return (GFARM_ERR_FRAGMENT_INDEX_NOT_AVAILABLE);
	*nfragmentsp = gf->pi.status.st_nsections;
	return (GFARM_ERR_NO_ERROR);
}

#endif /* not yet in gfarm v2 */

gfarm_error_t
gfs_pio_close(GFS_File gf)
{
	gfarm_error_t e, e_save;
	gfarm_timerval_t t1, t2;

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

	/*
	 * no need to check and set the default file view here
	 * because neither gfs_pio_flush nor view_close is not
	 * needed unless the file view is specified by some
	 * operation.
	 */
	e_save = GFARM_ERR_NO_ERROR;
	if (gfs_pio_is_view_set(gf)) {
		if ((gf->mode & GFS_FILE_MODE_WRITE) != 0)
			e_save = gfs_pio_flush(gf);
		e = (*gf->ops->view_close)(gf);
		if (e_save == GFARM_ERR_NO_ERROR)
			e_save = e;
	}

	e = gfm_close_fd(gfs_pio_fileno(gf));
	if (e_save == GFARM_ERR_NO_ERROR)
		e_save = e;
	gfs_file_free(gf);

	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(gfs_pio_close_time += gfarm_timerval_sub(&t2, &t1));

	return (e_save);
}

static gfarm_error_t
gfs_pio_purge(GFS_File gf)
{
	gf->offset += gf->p;
	gf->p = gf->length = 0;
	return (GFARM_ERR_NO_ERROR);
}

#define CHECK_WRITABLE(gf) { \
	if (((gf)->mode & GFS_FILE_MODE_WRITE) == 0) \
		return (gfarm_errno_to_error(EBADF)); \
	else if ((gf)->error == GFARM_ERRMSG_GFS_PIO_IS_EOF) \
		(gf)->error = GFARM_ERR_NO_ERROR; \
}
/*
 * we check this against gf->open_flags rather than gf->mode,
 * because we may set GFARM_FILE_MODE_READ even if write-only case.
 */
#define CHECK_READABLE(gf) { \
	if (((gf)->open_flags & GFARM_FILE_ACCMODE) == GFARM_FILE_WRONLY) \
		return (gfarm_errno_to_error(EBADF)); \
}

#define CHECK_READABLE_EOF(gf) { \
	if (((gf)->open_flags & GFARM_FILE_ACCMODE) == GFARM_FILE_WRONLY) \
		return (EOF); \
}

static gfarm_error_t
gfs_pio_fillbuf(GFS_File gf, size_t size)
{
	gfarm_error_t e;
	size_t len;

	CHECK_READABLE(gf);

	if (gf->error != GFARM_ERR_NO_ERROR) /* error or EOF? */
		return (GFS_PIO_ERROR(gf));
	if (gf->p < gf->length)
		return (GFARM_ERR_NO_ERROR);

	if ((gf->mode & GFS_FILE_MODE_BUFFER_DIRTY) != 0) {
		e = gfs_pio_flush(gf);
		if (e != GFARM_ERR_NO_ERROR)
			return (e);
	} else
		gfs_pio_purge(gf);

	assert(gf->io_offset == gf->offset);

	e = (*gf->ops->view_pread)(gf, gf->buffer, size, gf->io_offset, &len);
	if (e != GFARM_ERR_NO_ERROR) {
		gf->error = e;
		return (e);
	}
	gf->length = len;
	gf->io_offset += len;
	if (len == 0)
		gf->error = GFARM_ERRMSG_GFS_PIO_IS_EOF;
	return (GFARM_ERR_NO_ERROR);
}

/* unlike other functions, this returns `*writtenp' even if an error happens */
static gfarm_error_t
do_write(GFS_File gf, const char *buffer, size_t length,
	size_t *writtenp)
{
	gfarm_error_t e = GFARM_ERR_NO_ERROR;
	size_t written, len;

	if (length == 0) {
		*writtenp = 0;
		return (GFARM_ERR_NO_ERROR);
	}
	if (gf->io_offset != gf->offset) {
		/* this happens when switching from reading to writing */
		gf->mode &= ~GFS_FILE_MODE_CALC_DIGEST;
		gf->io_offset = gf->offset;
	}
	for (written = 0; written < length; written += len) {
		e = (*gf->ops->view_pwrite)(
			gf, buffer + written, length - written, gf->io_offset,
			&len);
		if (e != GFARM_ERR_NO_ERROR) {
			gf->error = e;
			break;
		}
		gf->io_offset += len;
	}
	*writtenp = written;
	return (e);
}

gfarm_error_t
gfs_pio_flush(GFS_File gf)
{
	gfarm_error_t e = gfs_pio_check_view_default(gf);
	size_t written;

	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	CHECK_WRITABLE(gf);

	if ((gf->mode & GFS_FILE_MODE_BUFFER_DIRTY) != 0) {
		e = do_write(gf, gf->buffer, gf->length, &written);
		if (e != GFARM_ERR_NO_ERROR)
			return (e);
		gf->mode &= ~GFS_FILE_MODE_BUFFER_DIRTY;
	}
	if (gf->p >= gf->length)
		gfs_pio_purge(gf);

	return (GFARM_ERR_NO_ERROR);
}

gfarm_error_t
gfs_pio_seek(GFS_File gf, gfarm_off_t offset, int whence, gfarm_off_t *resultp)
{
	gfarm_error_t e;
	gfarm_off_t where;
	gfarm_timerval_t t1, t2;

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

	e = gfs_pio_check_view_default(gf);
	if (e != GFARM_ERR_NO_ERROR)
		goto finish;

	if (gf->error == GFARM_ERRMSG_GFS_PIO_IS_EOF)
		gf->error = GFARM_ERR_NO_ERROR;

	switch (whence) {
	case GFARM_SEEK_SET:
		where = offset;
		break;
	case GFARM_SEEK_CUR:
		where = offset + gf->offset + gf->p;
		break;
	case GFARM_SEEK_END:
		/* XXX FIXME: ask the file size to gfmd. */
		e = gf->error = GFARM_ERR_FUNCTION_NOT_IMPLEMENTED;
		goto finish;
	default:
		e = gf->error = GFARM_ERR_INVALID_ARGUMENT;
		goto finish;
	}
	if (where < 0) {
		e = gf->error = GFARM_ERR_INVALID_ARGUMENT;
		goto finish;
	}

	/*
	 * This is the case that the file offset will be repositioned
	 * within the current io buffer.
	 */
	if (gf->offset <= where && where <= gf->offset + gf->length) {
		/*
		 * We don't have to clear GFS_FILE_MODE_CALC_DIGEST bit here,
		 * because this is no problem to calculate checksum for
		 * write-only or read-only case.
		 * This is also ok on switching from writing to reading.
		 * This is not ok on switching from reading to writing,
		 * but gfs_pio_flush() clears the bit at that case.
		 */
		gf->p = where - gf->offset;
		if (resultp != NULL)
			*resultp = where;

		e = GFARM_ERR_NO_ERROR;
		goto finish;
	}

	gf->mode &= ~GFS_FILE_MODE_CALC_DIGEST;

	if (gf->mode & GFS_FILE_MODE_BUFFER_DIRTY) {
		e = gfs_pio_flush(gf);
		if (e != GFARM_ERR_NO_ERROR) {
			gf->error = e;
			goto finish;
		}
	}

	e = gf->error = GFARM_ERR_NO_ERROR; /* purge EOF/error state */
	gfs_pio_purge(gf);
	gf->offset = gf->io_offset = where;
	if (resultp != NULL)
		*resultp = where;

 finish:
	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(gfs_pio_seek_time += gfarm_timerval_sub(&t2, &t1));

	return (e);
}

gfarm_error_t
gfs_pio_truncate(GFS_File gf, gfarm_off_t length)
{
	gfarm_error_t e;
	gfarm_timerval_t t1, t2;

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

	e = gfs_pio_check_view_default(gf);
	if (e != GFARM_ERR_NO_ERROR)
		goto finish;

	CHECK_WRITABLE(gf);

	gf->mode &= ~GFS_FILE_MODE_CALC_DIGEST;

	if (gf->mode & GFS_FILE_MODE_BUFFER_DIRTY) {
		e = gfs_pio_flush(gf);
		if (e != GFARM_ERR_NO_ERROR)
			goto finish;
	}

	gf->error = GFARM_ERR_NO_ERROR; /* purge EOF/error state */
	gfs_pio_purge(gf);

	e = (*gf->ops->view_ftruncate)(gf, length);
	if (e != GFARM_ERR_NO_ERROR)
		gf->error = e;
finish:
	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(gfs_pio_truncate_time += gfarm_timerval_sub(&t2, &t1));

	return (e);
}

gfarm_error_t
gfs_pio_read(GFS_File gf, void *buffer, int size, int *np)
{
	gfarm_error_t e;
	char *p = buffer;
	int n = 0;
	int length;
	gfarm_timerval_t t1, t2;

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

	e = gfs_pio_check_view_default(gf);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	CHECK_READABLE(gf);

	while (size > 0) {
		if ((e = gfs_pio_fillbuf(gf,
		    ((gf->open_flags & GFARM_FILE_UNBUFFERED) &&
		    size < GFS_FILE_BUFSIZE) ? size : GFS_FILE_BUFSIZE))
		    != GFARM_ERR_NO_ERROR)
			break;
		if (gf->error != GFARM_ERR_NO_ERROR) /* EOF or error */
			break;
		length = gf->length - gf->p;
		if (length > size)
			length = size;
		memcpy(p, gf->buffer + gf->p, length);
		p += length;
		n += length;
		size -= length;
		gf->p += length;
	}
	if (e != GFARM_ERR_NO_ERROR && n == 0)
		goto finish;

	if (gf->open_flags & GFARM_FILE_UNBUFFERED)
		gfs_pio_purge(gf);
	*np = n;

	e = GFARM_ERR_NO_ERROR;
 finish:
	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(gfs_pio_read_time += gfarm_timerval_sub(&t2, &t1));

	return (e);
}

gfarm_error_t
gfs_pio_write(GFS_File gf, const void *buffer, int size, int *np)
{
	gfarm_error_t e;
	size_t written;
	gfarm_timerval_t t1, t2;

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

	e = gfs_pio_check_view_default(gf);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	CHECK_WRITABLE(gf);

	if (size + gf->p > GFS_FILE_BUFSIZE) {
		/*
		 * gf->buffer[gf->p .. GFS_FILE_BUFSIZE-1] will be overridden
		 * by buffer.
		 */
		gf->length = gf->p;
		e = gfs_pio_flush(gf); /* this does purge too */
		if (e != GFARM_ERR_NO_ERROR)
			goto finish;
	}
	if (size >= GFS_FILE_BUFSIZE) {
		/* shortcut to avoid unnecessary memory copy */
		assert(gf->p == 0); /* gfs_pio_flush() was called above */
		gf->length = 0;
		gf->mode &= ~GFS_FILE_MODE_BUFFER_DIRTY;

		e = do_write(gf, buffer, size, &written);
		if (e != GFARM_ERR_NO_ERROR && written == 0)
			goto finish;
		gf->offset += written;
		*np = written; /* XXX - size_t vs int */

		e = GFARM_ERR_NO_ERROR;
		goto finish;
	}
	gf->mode |= GFS_FILE_MODE_BUFFER_DIRTY;
	memcpy(gf->buffer + gf->p, buffer, size);
	gf->p += size;
	if (gf->p > gf->length)
		gf->length = gf->p;
	*np = size;
	e = GFARM_ERR_NO_ERROR;
	/* Commented by TRAN, we disabled cache on write
	if (gf->open_flags & GFARM_FILE_UNBUFFERED ||
	    gf->p >= GFS_FILE_BUFSIZE) {
		e = gfs_pio_flush(gf);
		if (gf->open_flags & GFARM_FILE_UNBUFFERED)
			gfs_pio_purge(gf);
	}
	*/
	//TRAN begin
	e = gfs_pio_flush(gf);
		gfs_pio_purge(gf);
	//TRAN end
 finish:
	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(gfs_pio_write_time += gfarm_timerval_sub(&t2, &t1));

	return (e);
}

//TRAN begin
gfarm_error_t
gfs_get_active_version(GFS_File gf,size_t *nversion)
{

      	gfarm_error_t e;

	e = gfs_pio_check_view_default(gf);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);
        *nversion = gf->active_version;
     
        e = GFARM_ERR_NO_ERROR;
        return (e);

}
gfarm_error_t
gfs_get_latest_version(GFS_File gf,size_t *nversion)
{
	gfarm_error_t e;

	e = gfs_pio_check_view_default(gf);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	e = (*gf->ops->view_fget_version_count)(gf, nversion);
	if (e != GFARM_ERR_NO_ERROR)
			gf->error = e;
        return (e);

}

// switch (nversion)
//	case -1: fixed the active version at setting time
//	case 0: the active version dynamically changed to the latest version at setting time
gfarm_error_t
gfs_set_version(GFS_File gf,size_t nversion, size_t * nactive_version)
{
  	gfarm_error_t e;

	e = gfs_pio_check_view_default(gf);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);
        //flushing and cleaning up the buffer
	if (nversion == 0)
	  gf->open_flags |= GFARM_FILE_UNBUFFERED;
        
	gfs_pio_flush(gf);
        gfs_pio_purge(gf);   
        
	e = (*gf->ops->view_fset_version)(gf, nversion, nactive_version);
	if (e != GFARM_ERR_NO_ERROR)
			gf->error = e;
        return (e);

}

gfarm_error_t
gfs_pio_vread(size_t nversion,GFS_File gf, void *buffer, int size, int *np)
{
	gfarm_error_t e;
	e = gfs_pio_check_view_default(gf);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	CHECK_READABLE(gf);
    //clean up the buffer before switching to another version
    gfs_pio_purge(gf);
    gfs_set_version(gf,nversion, &nversion);
    return(gfs_pio_read(gf, buffer, size, np));
}
//TRAN end

static gfarm_error_t
sync_internal(GFS_File gf, int operation, double *time)
{
	gfarm_error_t e;
	gfarm_timerval_t t1, t2;

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

	e = gfs_pio_flush(gf);
	if (e != GFARM_ERR_NO_ERROR)
		goto finish;

	e = (*gf->ops->view_fsync)(gf, operation);
	if (e != GFARM_ERR_NO_ERROR)
		gf->error = e;
finish:
	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(*time += gfarm_timerval_sub(&t2, &t1));

	return (e);
}

gfarm_error_t
gfs_pio_sync(GFS_File gf)
{
	return (sync_internal(gf, GFS_PROTO_FSYNC_WITH_METADATA,
			      &gfs_pio_sync_time));
}

gfarm_error_t
gfs_pio_datasync(GFS_File gf)
{
	return (sync_internal(gf, GFS_PROTO_FSYNC_WITHOUT_METADATA,
			      &gfs_pio_datasync_time));
}

int
gfs_pio_getc(GFS_File gf)
{
	gfarm_error_t e;
	int c;
	gfarm_timerval_t t1, t2;

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

	e = gfs_pio_check_view_default(gf);
	if (e != GFARM_ERR_NO_ERROR) {
		gf->error = e;
		return (EOF);
	}

	CHECK_READABLE_EOF(gf);

	if (gf->p >= gf->length) {
		if (gfs_pio_fillbuf(gf,
		    gf->open_flags & GFARM_FILE_UNBUFFERED ?
		    1 : GFS_FILE_BUFSIZE) != GFARM_ERR_NO_ERROR) {
			c = EOF; /* can get reason via gfs_pio_error() */
			goto finish;
		}
		if (gf->error != GFARM_ERR_NO_ERROR) {
			c = EOF;
			goto finish;
		}
	}
	c = ((unsigned char *)gf->buffer)[gf->p++];
	if (gf->open_flags & GFARM_FILE_UNBUFFERED)
		gfs_pio_purge(gf);
 finish:
	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(gfs_pio_getc_time += gfarm_timerval_sub(&t2, &t1));
	return (c);
}

int
gfs_pio_ungetc(GFS_File gf, int c)
{
	gfarm_error_t e = gfs_pio_check_view_default(gf);

	if (e != GFARM_ERR_NO_ERROR) {
		gf->error = e;
		return (EOF);
	}

	CHECK_READABLE_EOF(gf);

	if (c != EOF) {
		if (gf->p == 0) { /* cannot unget - XXX should permit this? */
			gf->error = GFARM_ERR_NO_SPACE;
			return (EOF);
		}
		/* We do not mark this buffer dirty here. */
		gf->buffer[--gf->p] = c;
	}
	return (c);
}

gfarm_error_t
gfs_pio_putc(GFS_File gf, int c)
{
	gfarm_error_t e;
	gfarm_timerval_t t1, t2;

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

	e = gfs_pio_check_view_default(gf);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	CHECK_WRITABLE(gf);

	if (gf->p >= GFS_FILE_BUFSIZE) {
		gfarm_error_t e = gfs_pio_flush(gf); /* this does purge too */

		if (e != GFARM_ERR_NO_ERROR)
			goto finish;
	}
	gf->mode |= GFS_FILE_MODE_BUFFER_DIRTY;
	gf->buffer[gf->p++] = c;
	if (gf->p > gf->length)
		gf->length = gf->p;
	if (gf->open_flags & GFARM_FILE_UNBUFFERED ||
	    gf->p >= GFS_FILE_BUFSIZE) {
		e = gfs_pio_flush(gf);
		if (gf->open_flags & GFARM_FILE_UNBUFFERED)
			gfs_pio_purge(gf);
	}
 finish:
	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(gfs_pio_putc_time += gfarm_timerval_sub(&t2, &t1));
	return (e);
}

/* mostly compatible with fgets(3) */
gfarm_error_t
gfs_pio_puts(GFS_File gf, const char *s)
{
	gfarm_error_t e = gfs_pio_check_view_default(gf);

	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	CHECK_WRITABLE(gf);

	while (*s != '\0') {
		gfarm_error_t e = gfs_pio_putc(gf, *(unsigned char *)s);

		if (e != GFARM_ERR_NO_ERROR)
			return (e);
		s++;
	}
	return (GFARM_ERR_NO_ERROR);
}

/* mostly compatible with fgets(3), but EOF check is done by *s == '\0' */
gfarm_error_t
gfs_pio_gets(GFS_File gf, char *s, size_t size)
{
	gfarm_error_t e = gfs_pio_check_view_default(gf);
	char *p = s;
	int c;
	gfarm_timerval_t t1, t2;

	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

#ifdef __GNUC__ /* workaround gcc warning: unused variable */
	c = EOF;
#endif
	CHECK_READABLE(gf);

	if (size <= 1) {
		gf->error = GFARM_ERR_INVALID_ARGUMENT;
		return (gf->error);
	}
	--size; /* for '\0' */
	for (; size > 0 && (c = gfs_pio_getc(gf)) != EOF; --size) {
		*p++ = c;
		if (c == '\n')
			break;
	}
	*p++ = '\0';

	gfs_profile(gfarm_gettimerval(&t2));
	/* XXX should introduce gfs_pio_gets_time??? */
	gfs_profile(gfs_pio_getline_time += gfarm_timerval_sub(&t2, &t1));

	return (GFARM_ERR_NO_ERROR);
}

gfarm_error_t
gfs_pio_getline(GFS_File gf, char *s, size_t size, int *eofp)
{
	gfarm_error_t e = gfs_pio_check_view_default(gf);
	char *p = s;
	int c;
	gfarm_timerval_t t1, t2;

	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

#ifdef __GNUC__ /* workaround gcc warning: unused variable */
	c = EOF;
#endif
	CHECK_READABLE(gf);

	if (size <= 1) {
		gf->error = GFARM_ERR_INVALID_ARGUMENT;
		return (gf->error);
	}
	--size; /* for '\0' */
	for (; size > 0 && (c = gfs_pio_getc(gf)) != EOF; --size) {
		if (c == '\n')
			break;
		*p++ = c;
	}
	*p++ = '\0';
	if (p == s + 1 && c == EOF) {
		*eofp = 1;
		return (GFS_PIO_ERROR(gf));
	}
	*eofp = 0;

	gfs_profile(gfarm_gettimerval(&t2));
	gfs_profile(gfs_pio_getline_time += gfarm_timerval_sub(&t2, &t1));

	return (GFARM_ERR_NO_ERROR);
}

gfarm_error_t
gfs_pio_putline(GFS_File gf, const char *s)
{
	gfarm_error_t e = gfs_pio_check_view_default(gf);

	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	CHECK_WRITABLE(gf);

	e = gfs_pio_puts(gf, s);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);
	return (gfs_pio_putc(gf, '\n'));
}

#define ALLOC_SIZE_INIT	220

/*
 * mostly compatible with getline(3) in glibc,
 * but there are the following differences:
 * 1. on EOF, *lenp == 0
 * 2. on error, *lenp isn't touched.
 */
gfarm_error_t
gfs_pio_readline(GFS_File gf, char **bufp, size_t *sizep, size_t *lenp)
{
	gfarm_error_t e = gfs_pio_check_view_default(gf);
	char *buf = *bufp, *p = NULL;
	size_t size = *sizep, len = 0;
	int c;
	size_t alloc_size;
	int overflow = 0;
	gfarm_timerval_t t1, t2;

	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

#ifdef __GNUC__ /* workaround gcc warning: unused variable */
	c = EOF;
#endif
	CHECK_READABLE(gf);

	if (buf == NULL || size <= 1) {
		if (size <= 1)
			size = ALLOC_SIZE_INIT;
		GFARM_REALLOC_ARRAY(buf, buf, size);
		if (buf == NULL)
			return (GFARM_ERR_NO_MEMORY);
	}
	for (;;) {
		c = gfs_pio_getc(gf);
		if (c == EOF)
			break;
		if (size <= len) {
			alloc_size = gfarm_size_add(&overflow, size, size);
			if (!overflow)
				GFARM_REALLOC_ARRAY(p, buf, alloc_size);
			if (overflow || p == NULL) {
				*bufp = buf;
				*sizep = size;
				return (GFARM_ERR_NO_MEMORY);
			}
			buf = p;
			size += size;
		}
		buf[len++] = c;
		if (c == '\n')
			break;
	}
	if (size <= len) {
		alloc_size = gfarm_size_add(&overflow, size, size);
		if (!overflow)
			GFARM_REALLOC_ARRAY(p, buf, alloc_size);
		if (overflow || p == NULL) {
			*bufp = buf;
			*sizep = size;
			return (GFARM_ERR_NO_MEMORY);
		}
		buf = p;
		size += size;
	}
	buf[len] = '\0';

	gfs_profile(gfarm_gettimerval(&t2));
	/* XXX should introduce gfs_pio_readline_time??? */
	gfs_profile(gfs_pio_getline_time += gfarm_timerval_sub(&t2, &t1));

	*bufp = buf;
	*sizep = size;
	*lenp = len;

	return (GFARM_ERR_NO_ERROR);
}

/*
 * mostly compatible with getdelim(3) in glibc,
 * but there are the following differences:
 * 1. on EOF, *lenp == 0
 * 2. on error, *lenp isn't touched.
 */
gfarm_error_t
gfs_pio_readdelim(GFS_File gf, char **bufp, size_t *sizep, size_t *lenp,
	const char *delim, size_t delimlen)
{
	gfarm_error_t e = gfs_pio_check_view_default(gf);
	char *buf = *bufp, *p = NULL;
	size_t size = *sizep, len = 0, alloc_size;
	int c, delimtail, overflow;
	static const char empty_line[] = "\n\n";
	gfarm_timerval_t t1, t2;

	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	GFARM_TIMEVAL_FIX_INITIALIZE_WARNING(t1);
	gfs_profile(gfarm_gettimerval(&t1));

#ifdef __GNUC__ /* workaround gcc warning: unused variable */
	c = EOF;
#endif
	CHECK_READABLE(gf);

	if (delim == NULL) { /* special case 1 */
		delimtail = 0; /* workaround gcc warning */
	} else {
		if (delimlen == 0) { /* special case 2 */
			delim = empty_line;
			delimlen = 2;
		}
		delimtail = delim[delimlen - 1];
	}
	if (buf == NULL || size <= 1) {
		if (size <= 1)
			size = ALLOC_SIZE_INIT;
		GFARM_REALLOC_ARRAY(buf, buf, size);
		if (buf == NULL)
			return (GFARM_ERR_NO_MEMORY);
	}
	for (;;) {
		c = gfs_pio_getc(gf);
		if (c == EOF)
			break;
		if (size <= len) {
			alloc_size = gfarm_size_add(&overflow, size, size);
			if (!overflow)
				GFARM_REALLOC_ARRAY(p, buf, alloc_size);
			if (overflow || p == NULL) {
				*bufp = buf;
				*sizep = size;
				return (GFARM_ERR_NO_MEMORY);
			}
			buf = p;
			size += size;
		}
		buf[len++] = c;
		if (delim == NULL) /* special case 1: no delimiter */
			continue;
		if (len >= delimlen && c == delimtail &&
		    memcmp(&buf[len - delimlen], delim, delimlen) == 0) {
			if (delim == empty_line) { /* special case 2 */
				for (;;) {
					c = gfs_pio_getc(gf);
					if (c == EOF)
						break;
					if (c != '\n') {
						gfs_pio_ungetc(gf, c);
						break;
					}
					if (size <= len) {
						alloc_size = gfarm_size_add(
						    &overflow, size, size);
						if (!overflow)
							GFARM_REALLOC_ARRAY(p,
							    buf, alloc_size);
						if (overflow || p == NULL) {
							*bufp = buf;
							*sizep = size;
							return (
							  GFARM_ERR_NO_MEMORY);
						}
						buf = p;
						size += size;
					}
					buf[len++] = c;
				}
			}
			break;
		}
	}
	if (size <= len) {
		alloc_size = gfarm_size_add(&overflow, size, size);
		if (!overflow)
			GFARM_REALLOC_ARRAY(p, buf, alloc_size);
		if (overflow || p == NULL) {
			*bufp = buf;
			*sizep = size;
			return (GFARM_ERR_NO_MEMORY);
		}
		buf = p;
		size += size;
	}
	buf[len] = '\0';

	gfs_profile(gfarm_gettimerval(&t2));
	/* XXX should introduce gfs_pio_readdelim_time??? */
	gfs_profile(gfs_pio_getline_time += gfarm_timerval_sub(&t2, &t1));

	*bufp = buf;
	*sizep = size;
	*lenp = len;

	return (GFARM_ERR_NO_ERROR);
}

/*
 * fstat
 */

gfarm_error_t
gfs_pio_stat(GFS_File gf, struct gfs_stat *st)
{
	gfarm_error_t e;

	e = gfs_fstat(gf, st);
	if (e != GFARM_ERR_NO_ERROR)
		return (e);

	if (gfs_pio_is_view_set(gf)) {
		e = (*gf->ops->view_fstat)(gf, st);
		if (e != GFARM_ERR_NO_ERROR)
			gf->error = e;
	}
	return (e);
}

void
gfs_pio_display_timers(void)
{
	gflog_info("gfs_pio_create  : %g sec", gfs_pio_create_time);
	gflog_info("gfs_pio_open    : %g sec", gfs_pio_open_time);
	gflog_info("gfs_pio_close   : %g sec", gfs_pio_close_time);
	gflog_info("gfs_pio_seek    : %g sec", gfs_pio_seek_time);
	gflog_info("gfs_pio_truncate : %g sec", gfs_pio_truncate_time);
	gflog_info("gfs_pio_read    : %g sec", gfs_pio_read_time);
	gflog_info("gfs_pio_write   : %g sec", gfs_pio_write_time);
	gflog_info("gfs_pio_sync    : %g sec", gfs_pio_sync_time);
	gflog_info("gfs_pio_getline : %g sec (this calls getc)",
			gfs_pio_getline_time);
	gflog_info("gfs_pio_getc : %g sec", gfs_pio_getc_time);
	gflog_info("gfs_pio_putc : %g sec", gfs_pio_putc_time);
}
