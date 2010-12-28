#ifdef HAVE_FORTRAN

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sbs_common.h"
#include "sbs_types.h"
#include "sbs_client.h"

/* initialize a new client with a given thread id */
int sbs_initialize_(int64_t* handler, int* tid, int* ierr)
{
	*handler = (int64_t)sbs_initialize(*tid);
	*ierr = (*handler == 0);
	return 1;
}

/* start a new write-phase */
int sbs_start_step_(int64_t* handler, uint64_t* step)
{
	*step = sbs_start_step((sbs_client)(*handler));
	return 1;
}

/* end the current write-phase */
int sbs_end_step_(int64_t* handler, uint64_t* step)
{
	*step = sbs_end_step((sbs_client)(*handler));
	return 1;
}

/* write a variable in mailbox and send cmd to server */
int sbs_write_(int64_t* handler, const char* varname, void* data, int* type,
	      int* dim, uint64_t* offsets, uint64_t* lsizes, int* ierr,
              int varname_size)
{
	int i;

	char* newvarname = (char*)malloc(sizeof(char)*(varname_size+1));
	memcpy(newvarname,varname,varname_size);
	newvarname[varname_size] = '\0';

	uint64_t* newoffsets = (uint64_t*)malloc(sizeof(uint64_t)*(*dim));
	size_t* newlsizes = (size_t*)malloc(sizeof(size_t)*(*dim));
	
	for(i=0;i < *dim; i++) {
		newoffsets[i] = offsets[*dim-1-i];
	}

	for(i=0; i < *dim; i++) {
		newlsizes[i] = (size_t)lsizes[*dim-1-i];
	}

	*ierr = sbs_write((sbs_client)(*handler), newvarname, data, *type,
		          *dim, newoffsets, newlsizes);

	free(newvarname);
	free(newoffsets);
	free(newlsizes);

	return 1;
}

/* free the client, and shutdown server */
int sbs_finalize_(int64_t* handler)
{
	sbs_finalize((sbs_client)(*handler));
	return 1;
}

#endif 
