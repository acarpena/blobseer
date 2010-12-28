/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *
 *	Written by Viet-Trung TRAN
 */

#include "ad_bfs.h"
#include "adioi.h"

/* ADIOI_BFS_IreadContig()
 *
 * Implemented by immediately calling ReadContig()
 */
void ADIOI_BFS_IreadContig(ADIO_File fd, void *buf, int count,
			      MPI_Datatype datatype, int file_ptr_type,
			      ADIO_Offset offset, ADIO_Request *request, int
			      *error_code)
{
    ADIO_Status status;
    int myrank, nprocs, typesize, len;

    *error_code = MPI_SUCCESS;

    MPI_Comm_size(fd->comm, &nprocs);
    MPI_Comm_rank(fd->comm, &myrank);
    MPI_Type_size(datatype, &typesize);
    FPRINTF(stdout, "[%d/%d] ADIOI_BFS_IreadContig called on %s\n",
	    myrank, nprocs, fd->filename);
    FPRINTF(stdout, "[%d/%d]    calling ADIOI_BFS_ReadContig\n",
	    myrank, nprocs);

    len = count * typesize;
    ADIOI_BFS_ReadContig(fd, buf, len, MPI_BYTE, file_ptr_type,
			    offset, &status, error_code);

    MPIO_Completed_request_create(&fd, len, error_code, request);
}

void ADIOI_BFS_IreadStrided(ADIO_File fd, void *buf, int count,
			       MPI_Datatype datatype, int file_ptr_type,
			       ADIO_Offset offset, ADIO_Request *request, int
			       *error_code)
{
    ADIO_Status status;
    int myrank, nprocs;
    int typesize;

    MPI_Comm_size(fd->comm, &nprocs);
    MPI_Comm_rank(fd->comm, &myrank);
    MPI_Type_size(datatype, &typesize);
    FPRINTF(stdout, "[%d/%d] ADIOI_BFS_IreadStrided called on %s\n",
	    myrank, nprocs, fd->filename);
    FPRINTF(stdout, "[%d/%d]    calling ADIOI_BFS_ReadStrided\n",
	    myrank, nprocs);

    ADIOI_BFS_ReadStrided(fd, buf, count, datatype, file_ptr_type,
			     offset, &status, error_code); 
    MPIO_Completed_request_create(&fd, count*typesize, error_code, request);
}

