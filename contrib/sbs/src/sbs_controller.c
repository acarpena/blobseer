#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "sbs_common.h"
#include "sbs_cmd.h"
#include "sbs_controller.h"
#include "sbs_shmem.h"
#include "sbs_filter.h"
#include "sbs_variable.h"
#include "sbs_buffer.h"
#include "sbs_config.h"

#ifdef _ADIOS
#	include "sbs_writer_adios.h"
#endif
#ifdef _HDF5
#	include "sbs_writer_hdf5.h"
#endif
#ifdef _PHDF5
#	include "sbs_writer_phdf5.h"
#endif

/* this is the only function of a controller, that takes
 a shared segment descriptor and a ptr to a newly received cmd
 and process it */
int sbs_controller_process(MPI_Comm comm, sbs_shmem segment, struct sbs_cmd_s* cmd)
{
	switch(cmd->cmd_type){

	case CMD_EXIT: 
		return -1;

	case CMD_WRITE:
		/* this indicate that the client has sent some data */
		{
		DEBUG(printf("processing client write request through cmdq\n");)
		DEBUG(printf("   client is: %d\n",cmd->client_id);)
		DEBUG(printf("   cmd is: %d\n", cmd->cmd_type);)
		DEBUG(printf("   offset is: %ld\n",(long)(cmd->offset));)
		DEBUG(printf("   size is: %ld\n",(long)(cmd->size));)
		
		DEBUG(printf("will now try to read variable contents...\n");)
			
		uint64_t offset = cmd->offset; /* this is the offset where to start reading
										in the mailbox */
		size_t size = cmd->size; /* this is the size of data 
								  (actually it is useless) */
		int cid = cmd->client_id; /* the client id */
		
		/* buf is a ptr over the data section of the right mailbox,
		 to compute it we skip cmdq part of the segment, and then
		 we compute the right offset given the client id */
		char* buf = segment->data + sizeof(struct sbs_cmdq_data_s)
			+ sizeof(struct sbs_shared_mb_contents_s)*cid
			+ sizeof(uint64_t) + offset;
			
		// loading metadata
		size_t s; // size of the metadata
		sbs_variable v = sbs_variable_unserialize(buf,&s);
		DEBUG(sbs_variable_print(v);)
		v->data = buf+s; // raw data starts here
		//DEBUG(sbs_variable_print_data(v);)
			
		// now start processing...
		// does the variable exists?
		sbs_variable vout = sbs_buffer_find_var(v->name,v->time);
		if(vout == NULL)
		{
			printf("trying to write a variable which is not");
			printf(" defined in the buffer configuration\n");
			printf(" ------ %s\n",v->name);
		}
		else // in case the variable exists
		{	
			DEBUG(printf("doing something with variable %s...\n",
				vout->name);)
			sbs_copy(v,vout);
			// decrement the access counter
			int new_count = 
				sbs_buffer_decr_counter(v->name,v->time);
			DEBUG(printf("new count is %d\n",new_count);)
			// if the access counter is now 0 we will
			// have to flush the variable
			if(new_count == 0) {
				DEBUG(printf("we will have to process var\n");)
				//DEBUG(sbs_variable_print_data(vout);)

				int myrk; MPI_Comm_rank(comm,&myrk);
				int mpisize; MPI_Comm_size(comm,&mpisize);
				DEBUG(printf("[info] in rank %d, reached var-writing section, will write %s now\n",myrk,vout->name);)
#ifdef _ADIOS
				sbs_write_adio(comm,vout,fname);
#endif
#ifdef _HDF5
				sbs_write_hdf5(comm,vout);
#endif				
#ifdef _PHDF5
				char fname[256];
				sprintf(fname,"temp_%d.h5",(int)v->time);
				sbs_write_phdf5(vout,fname);
#endif
				MPI_Barrier(comm);
				sbs_buffer_free_var(vout->name,v->time);
			}
		}
		// try realign offset
	  	sbs_mailbox mb = sbs_shmem_map_mailbox(segment,cid);
		sbs_mailbox_realign(mb,offset+size);
		// free some variables
		free(mb);	
		free(v);
		return 0;
		}
	case CMD_STEP:
		{
		DEBUG(printf("cmd step received : starting step %ld\n",
				(long)(cmd->offset));)
		sbs_config_new_step((int)(cmd->offset));
#ifdef _HDF5
		int rk; MPI_Comm_rank(comm,&rk);
		sbs_writer_hdf5_end_step((int)(cmd->offset)-1);
		sbs_writer_hdf5_start_step((int)(cmd->offset),rk);
#endif
#ifdef _PHDF5
		sbs_writer_phdf5_end_step((int)(cmd->offset)-1);
		sbs_writer_phdf5_start_step((int)(cmd->offset));
#endif
		return 0;
		}
	default:
		return 0;
	}
}
