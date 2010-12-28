/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *	Written by Viet-Trung TRAN
 */


#include "adio.h"
#include "adio_extern.h"
#include "ad_bfs.h"
#include "adioi.h"

void ADIOI_BFS_WriteContig(ADIO_File fd, void *buf, int count,
		MPI_Datatype datatype, int file_ptr_type,
		ADIO_Offset offset, ADIO_Status *status, int
		*error_code)
{
	int datatype_size;
	BFS_File bfd;
	bfd = get_filep(fd);
	//////////////////////
	int err = -1;
	ADIO_Offset len;
	static char myname[] = "ADIOI_BFS_WRITECONTIG";

	MPI_Type_size(datatype, &datatype_size);
	len = (ADIO_Offset)datatype_size * (ADIO_Offset)count;
	ADIOI_Assert(len == (unsigned int) len); /* read takes an unsigned int parm */

	if (file_ptr_type == ADIO_INDIVIDUAL) {
		offset = fd->fp_ind;
	}

#ifdef ADIOI_MPE_LOGGING
	MPE_Log_event( ADIOI_MPE_write_a, 0, NULL );
#endif
	err = blob_write(&(bfd->blob), offset, len, buf);
	/* --BEGIN ERROR HANDLING-- */
	if (err == 0) {
		*error_code = MPIO_Err_create_code(MPI_SUCCESS,
				MPIR_ERR_RECOVERABLE,
				myname, __LINE__,
				MPI_ERR_IO, "**errorinblob_write",
				"**errorinblob_write %s", strerror(errno));
		fd->fp_sys_posn = -1;
		return;
	}
	/* --END ERROR HANDLING-- */

#ifdef ADIOI_MPE_LOGGING
	MPE_Log_event( ADIOI_MPE_write_b, 0, NULL );
#endif

	fd->fp_sys_posn = offset + len;

	if (file_ptr_type == ADIO_INDIVIDUAL) {
		fd->fp_ind += len;
	}

#ifdef HAVE_STATUS_SET_BYTES
	if (err != -1) MPIR_Status_set_bytes(status, datatype, err);
#endif

	*error_code = MPI_SUCCESS;

	/* --BEGIN LOGING-- */
	int myrank, nprocs;
	MPI_Comm_size(fd->comm, &nprocs);
	MPI_Comm_rank(fd->comm, &myrank);
	MPI_Type_size(datatype, &datatype_size);
	FPRINTF(stdout, "[%d/%d] ADIOI_TESTFS_WriteContig called on %s\n", myrank,
			nprocs, fd->filename);
	FPRINTF(stdout, "[%d/%d]    writing (buf = %p, loc = %lld, sz = %lld)\n",
			myrank, nprocs, buf, (long long) offset,
			(long long)datatype_size * (long long)count);
	/* --END LOGING-- */
}

void ADIOI_BFS_WriteStrided(ADIO_File fd, void *buf, int count,
		MPI_Datatype datatype, int file_ptr_type,
		ADIO_Offset offset, ADIO_Status *status,
		int *error_code)
{

	/* as with all the other WriteStrided functions, offset is in units of
	 * etype relative to the filetype */

	/* Since PVFS2 does not support file locking, can't do buffered writes
       as on Unix */

	ADIOI_Flatlist_node *flat_buf, *flat_file;
	int i, j, k, bwr_size, fwr_size=0, st_index=0;
	int bufsize, sum, n_etypes_in_filetype, size_in_filetype;
	int n_filetypes, etype_in_filetype;
	ADIO_Offset abs_off_in_filetype=0;
	int filetype_size, etype_size, buftype_size;
	MPI_Aint filetype_extent, buftype_extent;
	int buf_count, buftype_is_contig, filetype_is_contig;
	ADIO_Offset off, disp, start_off, initial_off;
	int flag, st_fwr_size, st_n_filetypes;
	int err_flag=0;

	int mem_list_count, file_list_count;
	uint32_t * mem_offsets;
	int64_t *file_offsets;
	int *mem_lengths;
	int32_t *file_lengths;
	int total_blks_to_write;

	int max_mem_list, max_file_list;

	int b_blks_wrote;
	int f_data_wrote;
	int size_wrote=0;

	int end_bwr_size, end_fwr_size;
	int start_k, start_j, new_file_write, new_buffer_write;
	int start_mem_offset;
	MPI_Offset total_bytes_written=0;
	static char myname[] = "ADIOI_PVFS2_WRITESTRIDED";

	/* note: don't increase this: several parts of PVFS2 now
	 * assume this limit*/
#define MAX_ARRAY_SIZE 64

	/* --BEGIN ERROR HANDLING-- */
	if (fd->atomicity) {
		*error_code = MPIO_Err_create_code(MPI_SUCCESS,
				MPIR_ERR_RECOVERABLE,
				myname, __LINE__,
				MPI_ERR_ARG,
				"Atomic noncontiguous writes are not supported by PVFS2", 0);
		return;
	}
	/* --END ERROR HANDLING-- */

	ADIOI_Datatype_iscontig(datatype, &buftype_is_contig);
	ADIOI_Datatype_iscontig(fd->filetype, &filetype_is_contig);

	/* the HDF5 tests showed a bug in this list processing code (see many many
	 * lines down below).  We added a workaround, but common HDF5 file types
	 * are actually contiguous and do not need the expensive workarond */
	if (!filetype_is_contig) {
		flat_file = ADIOI_Flatlist;
		while (flat_file->type != fd->filetype) flat_file = flat_file->next;
		if (flat_file->count == 1 && !buftype_is_contig)
			filetype_is_contig = 1;
	}

	MPI_Type_size(fd->filetype, &filetype_size);
	if ( ! filetype_size ) {
		*error_code = MPI_SUCCESS;
		return;
	}

	MPI_Type_extent(fd->filetype, &filetype_extent);
	MPI_Type_size(datatype, &buftype_size);
	MPI_Type_extent(datatype, &buftype_extent);
	etype_size = fd->etype_size;

	bufsize = buftype_size * count;

	if (!buftype_is_contig && filetype_is_contig) {
		FPRINTF(stdout, "noncontiguous in memory, contiguous in file. Unsupported yet\n");
		return;
	} /* if (!buftype_is_contig && filetype_is_contig) */

	/* already know that file is noncontiguous from above */
	/* noncontiguous in file */

	/* filetype already flattened in ADIO_Open */
	flat_file = ADIOI_Flatlist;
	while (flat_file->type != fd->filetype) flat_file = flat_file->next;

	disp = fd->disp;
	initial_off = offset;

	/* for each case - ADIO_Individual pointer or explicit, find offset
       (file offset in bytes), n_filetypes (how many filetypes into file
       to start), fwr_size (remaining amount of data in present file
       block), and st_index (start point in terms of blocks in starting
       filetype) */
	if (file_ptr_type == ADIO_INDIVIDUAL) {
		offset = fd->fp_ind; /* in bytes */
		n_filetypes = -1;
		flag = 0;
		while (!flag) {
			n_filetypes++;
			for (i=0; i<flat_file->count; i++) {
				if (disp + flat_file->indices[i] +
						((ADIO_Offset) n_filetypes)*filetype_extent +
						flat_file->blocklens[i] >= offset) {
					st_index = i;
					fwr_size = disp + flat_file->indices[i] +
							((ADIO_Offset) n_filetypes)*filetype_extent
							+ flat_file->blocklens[i] - offset;
					flag = 1;
					break;
				}
			}
		} /* while (!flag) */
	} /* if (file_ptr_type == ADIO_INDIVIDUAL) */
	else {
		n_etypes_in_filetype = filetype_size/etype_size;
		n_filetypes = (int) (offset / n_etypes_in_filetype);
		etype_in_filetype = (int) (offset % n_etypes_in_filetype);
		size_in_filetype = etype_in_filetype * etype_size;

		sum = 0;
		for (i=0; i<flat_file->count; i++) {
			sum += flat_file->blocklens[i];
			if (sum > size_in_filetype) {
				st_index = i;
				fwr_size = sum - size_in_filetype;
				abs_off_in_filetype = flat_file->indices[i] +
						size_in_filetype - (sum - flat_file->blocklens[i]);
				break;
			}
		}

		/* abs. offset in bytes in the file */
		offset = disp + ((ADIO_Offset) n_filetypes)*filetype_extent +
				abs_off_in_filetype;
	} /* else [file_ptr_type != ADIO_INDIVIDUAL] */

	start_off = offset;
	st_fwr_size = fwr_size;
	st_n_filetypes = n_filetypes;

	if (buftype_is_contig && !filetype_is_contig) {

		/* contiguous in memory, noncontiguous in file. should be the most
   common case. */

		int mem_lengths;
		char *mem_offsets;

		i = 0;
		j = st_index;
		off = offset;
		n_filetypes = st_n_filetypes;

		mem_list_count = 1;

		/* determine how many blocks in file to write */
		f_data_wrote = ADIOI_MIN(st_fwr_size, bufsize);
		total_blks_to_write = 1;
		if (j < (flat_file->count -1)) j++;
		else {
			j = 0;
			n_filetypes++;
		}
		while (f_data_wrote < bufsize) {
			f_data_wrote += flat_file->blocklens[j];
			total_blks_to_write++;
			if (j<(flat_file->count-1)) j++;
			else j = 0;
		}

		j = st_index;
		n_filetypes = st_n_filetypes;

		mem_offsets = buf;
		mem_lengths = 0;


		file_offsets = (int64_t*)ADIOI_Malloc(total_blks_to_write*
				sizeof(int64_t));
		file_lengths = (int32_t*)ADIOI_Malloc(total_blks_to_write*
				sizeof(int32_t));
		/* for file arrays smaller than MAX_ARRAY_SIZE (last write_list call) */
				if (total_blks_to_write) {
					file_list_count = total_blks_to_write;
					if(!i) {
						file_offsets[0] = offset;
						file_lengths[0] = ADIOI_MIN(st_fwr_size, bufsize);
					}
					for (k=0; k<total_blks_to_write; k++) {
						if(i || k) {
							file_offsets[k] = disp +
									((ADIO_Offset)n_filetypes)*filetype_extent +
									flat_file->indices[j];
							if (k == (total_blks_to_write - 1)) {
								file_lengths[k] = bufsize - (int32_t) mem_lengths
										- (int32_t) mem_offsets + (int32_t)  buf;
							}
							else file_lengths[k] = flat_file->blocklens[j];
						} /* if(i || k) */
						mem_lengths += file_lengths[k];
						if (j<(flat_file->count - 1)) j++;
						else {
							j = 0;
							n_filetypes++;
						}
					} /* for (k=0; k<extra_blks; k++) */

					/* as above, use 0 for 'offset' when using hindexed file type*/
#ifdef ADIOI_MPE_LOGGING
					MPE_Log_event( ADIOI_MPE_write_a, 0, NULL );
#endif
					FPRINTF(stdout, " contiguous in memory, noncontiguous in file. chunks needed: %d\n buf: %s \n", total_blks_to_write, mem_offsets);
					for (k = 0; k < total_blks_to_write; k++) {
						FPRINTF(stdout, "(offset,length) = (%d, %d) \n", file_offsets[k], file_lengths[k]);
					}

#ifdef ADIOI_MPE_LOGGING
					MPE_Log_event( ADIOI_MPE_write_b, 0, NULL );
#endif
					/* --BEGIN ERROR HANDLING-- */
					if (err_flag != 0) {
						/*		*error_code = MPIO_Err_create_code(MPI_SUCCESS,
						   MPIR_ERR_RECOVERABLE,
						   myname, __LINE__,
						   ADIOI_PVFS2_error_convert(err_flag),
						   "Error in PVFS_sys_write", 0);
						 */	goto error_state;
					}
					/* --END ERROR HANDLING-- */
					total_bytes_written += 0; //resp_io.total_completed;
				}
	}
	else {
		/* noncontiguous in memory as well as in file */
		FPRINTF(stdout, "noncontiguous in memory, noncontiguous in file. Unsupported yet\n");
		goto error_state;
	}
	/* when incrementing fp_ind, need to also take into account the file type:
	 * consider an N-element 1-d subarray with a lb and ub: ( |---xxxxx-----|
	 * if we wrote N elements, offset needs to point at beginning of type, not
	 * at empty region at offset N+1).
	 *
	 * As we discussed on mpich-discuss in may/june 2009, the code below might
	 * look wierd, but by putting fp_ind at the last byte written, the next
	 * time we run through the strided code we'll update the fp_ind to the
	 * right location. */
	if (file_ptr_type == ADIO_INDIVIDUAL) {
		fd->fp_ind = file_offsets[file_list_count-1]+
				file_lengths[file_list_count-1];
	}
	ADIOI_Free(file_offsets);
	ADIOI_Free(file_lengths);

	*error_code = MPI_SUCCESS;

	error_state:
	fd->fp_sys_posn = -1;   /* set it to null. */

#ifdef HAVE_STATUS_SET_BYTES
	MPIR_Status_set_bytes(status, datatype, bufsize);
	/* This is a temporary way of filling in status. The right way is to
   keep track of how much data was actually written by ADIOI_BUFFERED_WRITE. */
#endif

	if (!buftype_is_contig) ADIOI_Delete_flattened(datatype);

	///////////////////////////////////////////////////////////
	int myrank, nprocs;

	*error_code = MPI_SUCCESS;

	MPI_Comm_size(fd->comm, &nprocs);
	MPI_Comm_rank(fd->comm, &myrank);
	FPRINTF(stdout, "[%d/%d] ADIOI_BFS_WriteStrided called on %s\n",
			myrank, nprocs, fd->filename);
	FPRINTF(stdout, "[%d/%d]    calling ADIOI_GEN_WriteStrided\n",
			myrank, nprocs);

//	ADIOI_GEN_WriteStrided(fd, buf, count, datatype, file_ptr_type, offset,
//			status, error_code);
}
