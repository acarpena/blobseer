#ifdef _PHDF5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "hdf5.h"
#include "sbs_writer_phdf5.h"
#include "sbs_types.h"
#include "sbs_variable.h"

MPI_Comm comm;
hid_t big_file_id;
#define FILENAME "backup.h5"

int sbs_writer_phdf5_init(MPI_Comm c)
{
	comm = c;
	hid_t plist_id = H5Pcreate(H5P_FILE_ACCESS);
	MPI_Info info = MPI_INFO_NULL;
	H5Pset_fapl_mpio(plist_id,comm,info);
	big_file_id = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);
	H5Pclose(plist_id);
	return 0;
}

int sbs_writer_phdf5_finalize(MPI_Comm c)
{
	return 0;
}

int sbs_writer_phdf5_start_step(int step)
{
	return 0;
}

int sbs_writer_phdf5_end_step(int step)
{
	return 0;
}

int sbs_write_phdf5(sbs_variable v, char* filename)
{	
//	sbs_variable_print_data(v);
	if(v->type != SBS_FLOAT)
	{
		printf("WARN: hdf5 writer is not able to write this type (only float allowed in this prototype)\n");
		return -1;
	}
	hid_t file_id, dset_id;
	hid_t filespace, memspace;
	hsize_t* dimsf = malloc((1+v->dim)*sizeof(hsize_t));
	hsize_t* chunk_dims = malloc((1+v->dim)*sizeof(hsize_t));
	hsize_t* maxdims = malloc((1+v->dim)*sizeof(hsize_t));
	float* data = (float*)(v->data);
	
	hsize_t* count  = malloc((1+v->dim)*sizeof(hsize_t));
	hsize_t* stride = malloc((1+v->dim)*sizeof(hsize_t));
	hsize_t* block  = malloc((1+v->dim)*sizeof(hsize_t));
	hsize_t* offset = malloc((1+v->dim)*sizeof(hsize_t));
	
	hid_t plist_id;
	herr_t status;

	plist_id = H5Pcreate(H5P_FILE_ACCESS);
	MPI_Info info = MPI_INFO_NULL;
	H5Pset_fapl_mpio(plist_id, comm, info);

	file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);
	H5Pclose(plist_id);

	int i;
	dimsf[0] = 1;
	chunk_dims[0] = 1;
	maxdims[0] = H5S_UNLIMITED;
	for(i=0;i<v->dim;i++) 
	{
		maxdims[i+1] = v->gsizes[i];
		dimsf[i+1] = v->gsizes[i];
		chunk_dims[i+1] = v->lsizes[i];
	}
	
	filespace = H5Screate_simple(1+v->dim, dimsf, maxdims);
	memspace = H5Screate_simple(1+v->dim, chunk_dims, NULL);

	plist_id = H5Pcreate(H5P_DATASET_CREATE);
	H5Pset_chunk(plist_id, 1+v->dim, chunk_dims);

	char* dsetname = malloc(strlen(v->name)+2);
        strcpy(dsetname+1,v->name);
        dsetname[0] = '/';

	dset_id = H5Dcreate(file_id, dsetname, H5T_NATIVE_FLOAT, filespace, plist_id);
	H5Pclose(plist_id);
	H5Sclose(filespace);

	count[0] = 1;
	stride[0] = 1;
	block[0] = 1;
	offset[0] = v->time;

	for(i=0;i<v->dim;i++)
	{
		count[i+1]  = 1;
		stride[i+1] = 1;
		block[i+1] = chunk_dims[i];
		offset[i+1] = (v->offsets)[i];
	}
	
	filespace = H5Dget_space(dset_id);
	status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, stride, count, block);

	plist_id = H5Pcreate(H5P_DATASET_XFER);
	H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);

	status = H5Dwrite(dset_id, H5T_NATIVE_FLOAT, memspace, filespace, plist_id, data);

	H5Dclose(dset_id);
	H5Sclose(filespace);
	H5Sclose(memspace);
	H5Pclose(plist_id);
	H5Fclose(file_id);

	free(maxdims);
	free(dsetname);
	free(offset);
	free(count);
	free(stride);
	free(block);
	free(chunk_dims);
	free(dimsf);
	return 0;
}

#endif
