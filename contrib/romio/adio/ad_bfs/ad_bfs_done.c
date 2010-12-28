/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *
 *	Written by Viet-Trung TRAN
 */

#include "ad_bfs.h"
#include "adioi.h"

int ADIOI_BFS_ReadDone(ADIO_Request *request, ADIO_Status *status, int
			  *error_code)
{
    int myrank, nprocs;

    *error_code = MPI_SUCCESS;

    MPI_Comm_size( MPI_COMM_WORLD, &nprocs );
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
    FPRINTF(stdout, "[%d/%d] ADIOI_BFS_ReadDone called on ADIO_REQUEST_NULL\n",
	myrank, nprocs);
    return 1;
}


int ADIOI_BFS_WriteDone(ADIO_Request *request, ADIO_Status *status, int
			   *error_code)
{
    int myrank, nprocs;

    *error_code = MPI_SUCCESS;

    MPI_Comm_size( MPI_COMM_WORLD, &nprocs );
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
    FPRINTF(stdout, 
		"[%d/%d] ADIOI_BFS_WriteDone called on ADIO_REQUEST_NULL\n",
		myrank, nprocs);
    return 1;
}
