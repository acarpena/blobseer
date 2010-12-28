#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include "mpi.h"

#include "hdf5.h"

#include "sbs_writer.h"

#include "sbs_types.h"

#include "sbs_variable.h"



int sbs_writer_hdf5_init(MPI_Comm comm, char* config)

{

	return 0;

}



int sbs_writer_hdf5_finalize(MPI_Comm comm)

{

	return 0;

}



int sbs_write_hdf5(MPI_Comm comm, sbs_variable v, char* filename)

{

	if(v->type != SBS_FLOAT)

	{

		printf("WARN: hdf5 writer is not able to write this type\n");

		return -1;

	}



 	hid_t file_id, dataset_id, dataspace_id;

	hsize_t* dims = malloc(v->dim*sizeof(hsize_t));



	int i; for(i=0;i<v->dim;i++) dims[i] = v->lsizes[i];

	herr_t status;

	

	char* dsetname = malloc(strlen(v->name)+2);

	strcpy(dsetname+1,v->name);

	dsetname[0] = '/';



	file_id = H5Fcreate(filename, H5F_ACC_TRUNC, 

			H5P_DEFAULT, H5P_DEFAULT);



	dataspace_id = H5Screate_simple(v->dim, dims, NULL);



	dataset_id = H5Dcreate(file_id, dsetname, 

			H5T_NATIVE_FLOAT, dataspace_id,

			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	

	status = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,

			  v->data);



	status = H5Dclose(dataset_id);

	status = H5Sclose(dataspace_id);

	status = H5Fclose(file_id);

	

	free(dsetname);

	free(dims);

	return 0;

}

