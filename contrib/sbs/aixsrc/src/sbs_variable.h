#ifndef SBS_VARIABLE_H

#define SBS_VARIABLE_H

#include <stdlib.h>

#include <stdint.h>



typedef struct sbs_variable_s {

	
	int type;    
	int dim;     
	uint64_t time;   
	char* name;  
	
	size_t* gsizes;    
	size_t* lsizes;    
	uint64_t* offsets; 
	
	char* data;  
} *sbs_variable;



char* sbs_variable_serialize(sbs_variable v, size_t *size);

sbs_variable sbs_variable_unserialize(char* buffer, size_t *size);

void sbs_variable_print(sbs_variable v);

void sbs_variable_print_data(sbs_variable v);

#endif

