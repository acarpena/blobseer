/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *
 *	Written by Viet-Trung TRAN
 */

#ifndef AD_BFS_INCLUDE
#define AD_BFS_INCLUDE

#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "adio.h"

#include "BFS_interface-c.h"
//#include "blobseer.h"

#define BFS_MALLOC(p)         ((p) = ADIOI_Malloc(sizeof(*(p))))

struct blobseer_file {
	blob_t blob;
	blob_env_t env;
};
/*
struct blobseer_file {
    blob_env_t env;
    ns_handler_t ns;
    fblob_t *fblob;
};
*/
typedef struct blobseer_file *BFS_File;

static inline BFS_File get_filep(ADIO_File fd)
{
        return (BFS_File) (uintptr_t) fd->fd_sys;
}

void ADIOI_BFS_Open(ADIO_File fd, int *error_code);
void ADIOI_BFS_Close(ADIO_File fd, int *error_code);
void ADIOI_BFS_ReadContig(ADIO_File fd, void *buf, int count,
			     MPI_Datatype datatype, int file_ptr_type,
			     ADIO_Offset offset, ADIO_Status *status, int
			     *error_code);
void ADIOI_BFS_WriteContig(ADIO_File fd, void *buf, int count,
			      MPI_Datatype datatype, int file_ptr_type,
			      ADIO_Offset offset, ADIO_Status *status, int
			      *error_code);   
void ADIOI_BFS_IwriteContig(ADIO_File fd, void *buf, int count,
			       MPI_Datatype datatype, int file_ptr_type,
			       ADIO_Offset offset, ADIO_Request *request, int
			       *error_code);   
void ADIOI_BFS_IreadContig(ADIO_File fd, void *buf, int count,
			      MPI_Datatype datatype, int file_ptr_type,
			      ADIO_Offset offset, ADIO_Request *request, int
			      *error_code);   
int ADIOI_BFS_ReadDone(ADIO_Request *request, ADIO_Status *status, int
			  *error_code);
int ADIOI_BFS_WriteDone(ADIO_Request *request, ADIO_Status *status, int
			   *error_code);
void ADIOI_BFS_ReadComplete(ADIO_Request *request, ADIO_Status *status, int
			       *error_code); 
void ADIOI_BFS_WriteComplete(ADIO_Request *request, ADIO_Status *status,
				int *error_code); 
void ADIOI_BFS_Fcntl(ADIO_File fd, int flag, ADIO_Fcntl_t *fcntl_struct,
			int *error_code); 
void ADIOI_BFS_WriteStrided(ADIO_File fd, void *buf, int count,
			       MPI_Datatype datatype, int file_ptr_type,
			       ADIO_Offset offset, ADIO_Status *status,
			       int *error_code);
void ADIOI_BFS_ReadStrided(ADIO_File fd, void *buf, int count,
			      MPI_Datatype datatype, int file_ptr_type,
			      ADIO_Offset offset, ADIO_Status *status, int
			      *error_code);
void ADIOI_BFS_WriteStridedColl(ADIO_File fd, void *buf, int count,
				   MPI_Datatype datatype, int file_ptr_type,
				   ADIO_Offset offset, ADIO_Status *status, int
				   *error_code);
void ADIOI_BFS_ReadStridedColl(ADIO_File fd, void *buf, int count,
				  MPI_Datatype datatype, int file_ptr_type,
				  ADIO_Offset offset, ADIO_Status *status, int
				  *error_code);
void ADIOI_BFS_IreadStrided(ADIO_File fd, void *buf, int count,
			       MPI_Datatype datatype, int file_ptr_type,
			       ADIO_Offset offset, ADIO_Request *request, int
			       *error_code);
void ADIOI_BFS_IwriteStrided(ADIO_File fd, void *buf, int count,
				MPI_Datatype datatype, int file_ptr_type,
				ADIO_Offset offset, ADIO_Request *request, int
				*error_code);
void ADIOI_BFS_Flush(ADIO_File fd, int *error_code);
void ADIOI_BFS_Resize(ADIO_File fd, ADIO_Offset size, int *error_code);
ADIO_Offset ADIOI_BFS_SeekIndividual(ADIO_File fd, ADIO_Offset offset,
					int whence, int *error_code);
void ADIOI_BFS_SetInfo(ADIO_File fd, MPI_Info users_info, int *error_code);
void ADIOI_BFS_Get_shared_fp(ADIO_File fd, int size,
				ADIO_Offset *shared_fp, 
				int *error_code);
void ADIOI_BFS_Set_shared_fp(ADIO_File fd, ADIO_Offset offset,
				int *error_code);
void ADIOI_BFS_Delete(char *filename, int *error_code);

#endif




