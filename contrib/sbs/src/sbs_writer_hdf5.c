#ifdef _HDF5

#define min(a,b) a < b ? a : b

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "hdf5.h"
#include "sbs_common.h"
#include "sbs_writer_hdf5.h"
#include "sbs_types.h"
#include "sbs_variable.h"

struct hdf5_file_id_s;
int last_step;

typedef struct hdf5_file_id_s {
	int step;
	int wait_close;
	hid_t file_id;
	struct hdf5_file_id_s* next;
} *hdf5_file_id;

hdf5_file_id opened_files;

hdf5_file_id close_file_associated_with_step(hdf5_file_id list, int step)
{
	if(list == NULL) return NULL;
	if(list->step == step)
	{
		list->wait_close--;
		if(list->wait_close == 0)
		{
			DEBUG(printf("[hdf5] : closing file associated with step %d\n",step);)
			hdf5_file_id next = list->next;
			H5Fclose(list->file_id);
			free(list);
			return next;
		} else return list;
	}
	else
	{
		list->next = close_file_associated_with_step(list->next, step);
		return list;
	}
}

hdf5_file_id open_file_associated_with_step(hdf5_file_id list, int step, int rk)
{
	if(list == NULL)
	{
		hdf5_file_id elem = malloc(sizeof(struct hdf5_file_id_s));
		elem->step = step;
		elem->wait_close = SBS_CLIENT_NBR;
		elem->next = NULL;

		char filename[256];
		sprintf(filename,"backup_%d_%d.h5",rk,step);
		elem->file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
		DEBUG(printf("[hdf5] : opening file for step %d, file_id = %d\n",step,elem->file_id);)
		
		return elem;
	} else {
		if(list->step == step)
			return list;
		list->next = open_file_associated_with_step(list->next, step, rk);
		return list;		
	}
}

hid_t find_file_id_associated_with_step(hdf5_file_id list, int step)
{
	if(list == NULL) return 0;
	if(list->step == step) return list->file_id;
	else return find_file_id_associated_with_step(list->next, step);
}

int sbs_writer_hdf5_init(MPI_Comm comm)
{
	DEBUG(printf("[hdf5] : init\n");)
	last_step = -1;
	return 0;
}

int sbs_writer_hdf5_finalize(MPI_Comm comm)
{
	DEBUG(printf("[hdf5] : finalize\n");)
	return 0;
}

int sbs_writer_hdf5_start_step(int step, int rk)
{
	last_step = step;
	//printf("[hdf5] : start step %d\n", step);
	opened_files = open_file_associated_with_step(opened_files, step, rk);
	return 0;
}

int sbs_writer_hdf5_end_step(step)
{	
	//printf("[hdf5] : end step %d\n", step);
	opened_files = close_file_associated_with_step(opened_files, step);
	return 0;
}

int sbs_write_hdf5(MPI_Comm comm, sbs_variable v)
{
	if(v->type != SBS_FLOAT)
	{
		printf("WARN: hdf5 writer is not able to write this type (only float allowed in this prototype)\n");
		return -1;
	}
 	
	hid_t dataset_id, dataspace_id, chunk_id;
	hid_t file_id = find_file_id_associated_with_step(opened_files,v->time);
	DEBUG(printf("[hdf5] : writing, file_id is %ld, step is %d\n",file_id,v->time);)
	hsize_t* dims = malloc(v->dim*sizeof(hsize_t));
	hsize_t* chunkdims = malloc(v->dim*sizeof(hsize_t));
	unsigned int dummy_filter_values[2];
	unsigned int gzip_filter_values[1];
	gzip_filter_values[0] = 4;

	int i; for(i=0;i<v->dim;i++) 
	{
		dims[i] = v->lsizes[i];
		chunkdims[i] = v->lsizes[i];
	}
	chunkdims[v->dim-1] = v->lsizes[v->dim-1] % 2 == 0 ? min(2,v->lsizes[v->dim-1]) : 1;
	herr_t status;
	
	char* dsetname = malloc(strlen(v->name)+2);
	strcpy(dsetname+1,v->name);
	dsetname[0] = '/';	
	
	dataspace_id = H5Screate_simple(v->dim, dims, NULL);

	chunk_id = H5Pcreate(H5P_DATASET_CREATE);
	H5Pset_chunk(chunk_id,v->dim,chunkdims);
	
	//H5Pset_filter(chunk_id,2,0,0,dummy_filter_values);
	H5Pset_filter(chunk_id,1,0,1,gzip_filter_values);
	
	dataset_id = H5Dcreate(file_id, dsetname, 
			H5T_NATIVE_FLOAT, dataspace_id, chunk_id);
			//H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	
	status = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
			  v->data);

	status = H5Dclose(dataset_id);
	status = H5Sclose(dataspace_id);
	
	free(dsetname);
	free(dims);
	free(chunkdims);
        DEBUG(printf("[debug] just writen %s\n",v->name);)
	return 0;
}

#endif
