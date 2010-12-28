#ifndef SBS_BUFFER_H

#define SBS_BUFFER_H



#include "sbs_variable.h"



struct sbs_var_node_s;



typedef struct sbs_buffer_s {

	struct sbs_var_node_s {

		sbs_variable var; 
		int count; 
		int mpi_flag; 
		struct sbs_var_node_s* next;

	} *varlist;

}* sbs_buffer;



sbs_buffer sbs_buffer_get_instance();

/* sbs_buffer_create initialize the global buffer by

 allocating memory for it and initializing fields */

sbs_buffer sbs_buffer_create();

/* free the global buffer */

int sbs_buffer_free();

/* add a variable entry if it is not already defined */

sbs_variable sbs_buffer_define_var( 

			       char* name, int type, int dim,

			       size_t* lsizes, size_t* gsizes,

			       uint64_t* offsets, int time, int count);

/* sbs_buffer_free_var finds a particular variable entry

 given its name and remove it from the buffer */

int sbs_buffer_free_var(char* name, int time);

/* find a variable entry given its name and returns it.

 return NULL is no such entry exists */

sbs_variable sbs_buffer_find_var(char* name, int time);

/* decrement the counter associated with a given variable.

 This is used to know how many times we have still to access

 this variable before flushing it into files */

int sbs_buffer_decr_counter(char* name, int time);

#endif

