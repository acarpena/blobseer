/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *
 *	Written by Viet-Trung TRAN
 */

#include "ad_bfs.h"
#include "adioi.h"

void ADIOI_BFS_Open(ADIO_File fd, int *error_code)
{
	int err, myrank, nprocs;
	static char myname[] = "ADIOI_BFS_OPEN";

	// Test Blobseer
	BFS_File bfd;
	BFS_MALLOC(bfd);
	char * strconfig = BLOBSEER_CONFIGURATION;

#ifdef ADIOI_MPE_LOGGING
	MPE_Log_event( ADIOI_MPE_open_a, 0, NULL );
#endif
	blob_init(strconfig, &(bfd->env));
	if (fd->access_mode & ADIO_CREATE)
		err = blob_fcreate(&(bfd->env), fd->filename, 65536, 1, &(bfd->blob));
	else
		err = blob_setid(&(bfd->env),fd->filename, &(bfd->blob));

	/* --BEGIN ERROR HANDLING-- */
	if (err == 0)
	{
		*error_code = MPIO_Err_create_code(MPI_SUCCESS,
				MPIR_ERR_RECOVERABLE, myname,
				__LINE__, MPI_ERR_NO_SUCH_FILE,
				"**filenoexist",
				"**filenoexist %s",
				fd->filename);

	}
	/* --END ERROR HANDLING-- */
	else
	{
		*error_code = MPI_SUCCESS;
		fd->fd_sys = (unsigned long) bfd;
		FPRINTF(stdout, "on open %ld:%ld:%ld:%ld\n", (unsigned long)fd->fd_sys, (unsigned long)bfd, (unsigned long) &(bfd->env), (unsigned long) &(bfd->blob));
		fd->fd_direct = -1;
		fd->fp_ind = fd->fp_sys_posn = 0;
	}

#ifdef ADIOI_MPE_LOGGING
	MPE_Log_event( ADIOI_MPE_open_b, 0, NULL );
#endif

	MPI_Comm_size(fd->comm, &nprocs);
	MPI_Comm_rank(fd->comm, &myrank);
	FPRINTF(stdout, "[%d/%d] ADIOI_BFS_Open called on %s\n", myrank,
			nprocs, fd->filename);
}
