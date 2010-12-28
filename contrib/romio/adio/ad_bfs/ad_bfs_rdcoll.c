/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *
 *	Written by Viet-Trung TRAN
 */

#include "ad_bfs.h"
#include "adioi.h"

void ADIOI_BFS_ReadStridedColl(ADIO_File fd, void *buf, int count,
				  MPI_Datatype datatype, int file_ptr_type,
				  ADIO_Offset offset, ADIO_Status *status, 
				  int *error_code)
{
    int myrank, nprocs;

    *error_code = MPI_SUCCESS;

    MPI_Comm_size(fd->comm, &nprocs);
    MPI_Comm_rank(fd->comm, &myrank);
    FPRINTF(stdout, "[%d/%d] ADIOI_BFS_ReadStridedColl called on %s\n",
	    myrank, nprocs, fd->filename);    
    FPRINTF(stdout, "[%d/%d]    calling ADIOI_GEN_ReadStridedColl\n", 
	    myrank, nprocs);

    ADIOI_GEN_ReadStridedColl(fd, buf, count, datatype, file_ptr_type,
			      offset, status, error_code);
}
