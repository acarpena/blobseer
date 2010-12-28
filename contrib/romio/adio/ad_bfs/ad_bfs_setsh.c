/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *
 *	Written by Viet-Trung TRAN
 */

#include "ad_bfs.h"
#include "adioi.h"

void ADIOI_BFS_Set_shared_fp(ADIO_File fd, ADIO_Offset offset,
				int *error_code)
{
    int myrank, nprocs;

    *error_code = MPI_SUCCESS;

    MPI_Comm_size(fd->comm, &nprocs);
    MPI_Comm_rank(fd->comm, &myrank);
    FPRINTF(stdout, "[%d/%d] ADIOI_BFS_Set_shared_fp called on %s\n",
	    myrank, nprocs, fd->filename);
}
