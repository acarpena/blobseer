#ifdef _ADIOS

#include "mpi.h"
#include "adios.h"
#include "sbs_writer.h"
#include "sbs_types.h"
#include "sbs_variable.h"

int sbs_writer_adios_init(MPI_Comm comm, char* config)
{
	return adios_init(comm, config);
}

int sbs_writer_adios_finalize(MPI_Comm comm)
{
	int rank = 0;
	if(comm != MPI_COMM_NULL) MPI_Comm_rank(comm,&rank);
	return adios_finalize(rank);
}

int sbs_write_adios(MPI_Comm comm, sbs_variable v, char* filename)
{
    	uint64_t    adios_groupsize, adios_totalsize;
    	int64_t     adios_handle;
  
    	adios_open(&adios_handle, v->name, filename, "w", &comm);
    	adios_groupsize = sbs_sizeof(v->type);
	int i;
	for(i=0; i<v->dim; i++)
		adios_groupsize *= v->lsizes[i];
	adios_groupsize += (v->dim)*sizeof(size_t);
    	adios_group_size(adios_handle, adios_groupsize, &adios_totalsize);
	
	for(i=0;i<v->dim;i++)
	{
		char dname[4];
		dname[0] = 'l';
		dname[1] = 'd';
		dname[2] = (char)(49 + i);
		dname[3] = '\0';
        	adios_write (adios_handle, dname, &(v->lsizes[i]));
		dname[0] = 'g';
		adios_write (adios_handle, dname, &(v->gsizes[i]));
		char ofname[4];
		ofname[0] = 'o';
		ofname[1] = 'f';
		ofname[2] = (char)(49 + i);
		adios_write (adios_handle, ofname, &(v->offsets[i]));
	}
	adios_write (adios_handle, v->name, v->data);
	adios_close (adios_handle);
	return 0;
}

#endif
