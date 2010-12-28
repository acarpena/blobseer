/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *
 *	Written by Viet-Trung TRAN
 */

#include "ad_bfs.h"
#include "adioi.h"

void ADIOI_BFS_ReadComplete(ADIO_Request *request, ADIO_Status *status, int
			       *error_code)
{
    int myrank, nprocs;

    *error_code = MPI_SUCCESS;

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    FPRINTF(stdout, "[%d/%d] ADIOI_BFS_ReadComplete called \n",
	    myrank, nprocs);

    /* do something with status set bytes? */
}

void ADIOI_BFS_WriteComplete(ADIO_Request *request, ADIO_Status *status, int
				*error_code)
{
    int myrank, nprocs;

    *error_code = MPI_SUCCESS;

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    FPRINTF(stdout, "[%d/%d] ADIOI_BFS_WriteComplete called\n",
	    myrank, nprocs);

    /* do something with status_set_bytes? */
}
