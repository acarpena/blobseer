/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *
 *	Written by Viet-Trung TRAN
 */

#include "ad_bfs.h"
#include "adioi.h"

void ADIOI_BFS_Close(ADIO_File fd, int *error_code)
{
    int myrank, nprocs;
    static char myname[] = "ADIOI_BFS_CLOSE";

    // Test Blobseer
	BFS_File bfd;
#ifdef ADIOI_MPE_LOGGING
    MPE_Log_event( ADIOI_MPE_close_a, 0, NULL );
#endif
	bfd = get_filep(fd);
	FPRINTF(stdout, "on close %ld:%ld:%ld:%ld\n",(unsigned long)fd->fd_sys, (unsigned long)bfd, (unsigned long) &(bfd->env), (unsigned long) &(bfd->blob));

	blob_free(&(bfd->env),&(bfd->blob));
	blob_finalize(&(bfd->env));
	free(bfd);
#ifdef ADIOI_MPE_LOGGING
    MPE_Log_event( ADIOI_MPE_close_b, 0, NULL );
#endif
    fd->fd_direct =-1;
	fd->fd_sys = -1;
    *error_code = MPI_SUCCESS;

    MPI_Comm_size(fd->comm, &nprocs);
    MPI_Comm_rank(fd->comm, &myrank);
    FPRINTF(stdout, "[%d/%d] ADIOI_BFS_Close called on %s\n", myrank,
	    nprocs, fd->filename);
}
